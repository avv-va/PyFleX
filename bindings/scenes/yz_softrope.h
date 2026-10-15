class yz_SoftRope: public yz_XArmTabletop
{
public:

	yz_SoftRope(const char* name) : yz_XArmTabletop(name) {}

	struct RenderingInstance
	{
		Mesh* mMesh = nullptr;
		std::vector<int> mSkinningIndices;
		std::vector<float> mSkinningWeights;
		vector<Vec3> mRigidRestPoses;
		int mOffset;
	};

	RenderingInstance mRope;

	void Initialize(py::array_t<float> scene_params, int thread_idx = 0)
	{
		auto ptr = (float *) scene_params.request().ptr;

		Vec3 scale = Vec3(ptr[0], ptr[1], ptr[2]);
		Vec3 translation = Vec3(ptr[3], ptr[4], ptr[5]);
		float radius = ptr[6];
		float clusterSpacing = ptr[7];
		float clusterRadius = ptr[8];
		float clusterStiffness = ptr[9];
		float linkRadius = ptr[10];
		float linkStiffness = ptr[11];
		float globalStiffness = ptr[12];
		float surfaceSampling = ptr[13];
		float volumeSampling = ptr[14];
		float skinningFalloff = ptr[15];
		float skinningMaxDistance = ptr[16];
		float dynamicFriction = ptr[17];
		float particleFriction = ptr[18];
		float relaxationFactor = ptr[19];
		Quat rotation = Quat(ptr[20], ptr[21], ptr[22], ptr[23]);
		float collisionDistance = ptr[24];
		int numIterations = (int) ptr[25];
		int numSubsteps = (int) ptr[26];
		float pinHeight = ptr[27];
		float radiusExpansion = ptr[28];

		g_solverDesc.featureMode = eNvFlexFeatureModeSimpleSolids;

		g_params.radius = radius;
		g_params.dynamicFriction = dynamicFriction;
		g_params.particleFriction = particleFriction;
		g_params.numIterations = numIterations;
		g_params.collisionDistance = collisionDistance;
		g_params.relaxationFactor = relaxationFactor;
		g_numSubsteps = numSubsteps;
		g_windStrength = 0.0f;

		char path[200];
		strcpy(path, getenv("PYFLEXROOT"));
		strcat(path, "/data/rope.obj");

		Mesh* mesh = ImportMesh(GetFilePathByPlatform(path).c_str());
		mesh->Normalize();
		mesh->Transform(ScaleMatrix(scale*radius));
		mesh->Transform(RotationMatrix(rotation));
		mesh->Transform(TranslationMatrix(Point3(translation)));

		delete mRope.mMesh;  // the previous reset's rope mesh
		mRope.mMesh = mesh;
		mRope.mOffset = g_buffers->rigidTranslations.size();
		mRope.mRigidRestPoses.resize(0);

		NvFlexExtAsset* asset = NvFlexExtCreateSoftFromMesh(
			(float*)&mesh->m_positions[0],
			mesh->m_positions.size(),
			(int*)&mesh->m_indices[0],
			mesh->m_indices.size(),
			radius,
			volumeSampling,
			surfaceSampling,
			clusterSpacing*radius,
			clusterRadius*radius,
			clusterStiffness,
			linkRadius*radius,
			linkStiffness,
			globalStiffness,
			0.0f,
			0.0f);

		const int maxWeights = 4;
		mRope.mSkinningIndices.resize(mesh->m_positions.size()*maxWeights);
		mRope.mSkinningWeights.resize(mesh->m_positions.size()*maxWeights);

		for (int i = 0; i < asset->numShapes; ++i)
			mRope.mRigidRestPoses.push_back(Vec3(&asset->shapeCenters[i * 3]));

		NvFlexExtCreateSoftMeshSkinning(
			(float*)&mesh->m_positions[0],
			mesh->m_positions.size(),
			asset->shapeCenters,
			asset->numShapes,
			skinningFalloff,
			skinningMaxDistance,
			&mRope.mSkinningWeights[0],
			&mRope.mSkinningIndices[0]);

		if (g_buffers->rigidOffsets.empty())
			g_buffers->rigidOffsets.push_back(0);

		const int particleOffset = g_buffers->positions.size();
		const int indexOffset = g_buffers->rigidOffsets.back();

		for (int i = 0; i < asset->numParticles; ++i)
		{
			g_buffers->positions.push_back(&asset->particles[i * 4]);
			g_buffers->velocities.push_back(0.0f);
			g_buffers->phases.push_back(NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseSelfCollideFilter));
		}

		for (int i = 0; i < asset->numShapeIndices; ++i)
			g_buffers->rigidIndices.push_back(asset->shapeIndices[i] + particleOffset);

		for (int i = 0; i < asset->numShapes; ++i)
		{
			g_buffers->rigidOffsets.push_back(asset->shapeOffsets[i] + indexOffset);
			g_buffers->rigidTranslations.push_back(Vec3(&asset->shapeCenters[i * 3]));
			g_buffers->rigidRotations.push_back(Quat());
			g_buffers->rigidCoefficients.push_back(asset->shapeCoefficients[i]);
		}

		for (int i = 0; i < asset->numSprings; ++i)
		{
			g_buffers->springIndices.push_back(asset->springIndices[i * 2 + 0]);
			g_buffers->springIndices.push_back(asset->springIndices[i * 2 + 1]);
			g_buffers->springStiffness.push_back(asset->springCoefficients[i]);
			g_buffers->springLengths.push_back(asset->springRestLengths[i]);
		}

		NvFlexExtDestroyAsset(asset);

		for (int i = 0; i < int(g_buffers->positions.size()); ++i)
			if (g_buffers->positions[i].y < pinHeight)
				g_buffers->positions[i].w = 0.0f;

		g_params.radius *= radiusExpansion;

		g_drawPoints = false;
		g_drawMesh = true;
		g_wireframe = false;
		g_drawSprings = false;
		g_drawBases = false;
	}

	virtual void Draw(int pass)
	{
		if (!g_drawMesh)
			return;

		Mesh m;
		m.m_positions.resize(mRope.mMesh->m_positions.size());
		m.m_normals.resize(mRope.mMesh->m_normals.size());
		m.m_indices = mRope.mMesh->m_indices;

		for (int i = 0; i < int(mRope.mMesh->m_positions.size()); ++i)
		{
			Vec3 softPos;
			Vec3 softNormal;

			for (int w = 0; w < 4; ++w)
			{
				const int cluster = mRope.mSkinningIndices[i * 4 + w];
				const float weight = mRope.mSkinningWeights[i * 4 + w];

				if (cluster > -1)
				{
					int rigidIndex = cluster + mRope.mOffset;
					Vec3 localPos = Vec3(mRope.mMesh->m_positions[i]) - mRope.mRigidRestPoses[cluster];
					softPos += (g_buffers->rigidTranslations[rigidIndex] + Rotate(g_buffers->rigidRotations[rigidIndex], localPos))*weight;
					softNormal += Rotate(g_buffers->rigidRotations[rigidIndex], mRope.mMesh->m_normals[i])*weight;
				}
			}

			m.m_positions[i] = Point3(softPos);
			m.m_normals[i] = softNormal;
		}

		DrawMesh(&m, Vec3(0.5f, 0.5f, 1.0f));
	}
};
