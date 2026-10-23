class yz_BunnyPush: public yz_XArmTabletop
{
public:

	yz_BunnyPush(const char* name) : yz_XArmTabletop(name) {}

	void Initialize(py::array_t<float> scene_params, int thread_idx = 0)
	{
		auto ptr = (float *) scene_params.request().ptr;

		float spacing = ptr[0];
		float invMass = ptr[1];
		float dynamicFriction = ptr[2];
		float collisionDistance = ptr[3];
		int numIterations = (int) ptr[4];
		int numSubsteps = (int) ptr[5];
		float relaxationFactor = ptr[6];
		float dissipation = ptr[7];
		float sleepThresholdRatio = ptr[8];
		float shapeCollisionMargin = ptr[9];

		char path[200];
		strcpy(path, getenv("PYFLEXROOT"));
		strcat(path, "/data/bunny_push.obj");

		Mesh* bunnyMesh = ImportMesh(GetFilePathByPlatform(path).c_str());
		// the obj is authored +z up with its flat foot base at z=0; FleX's up axis is +y
		bunnyMesh->Transform(RotationMatrix(-DegToRad(90.0f), Vec3(1.0f, 0.0f, 0.0f)));

		Vec3 meshLower, meshUpper;
		bunnyMesh->GetBounds(meshLower, meshUpper);
		Vec3 edges = meshUpper - meshLower;
		float maxEdge = max(max(edges.x, edges.y), edges.z);

		// CreateParticleShape divides scale by the mesh's own maxEdge internally, so scale=Vec3(maxEdge) keeps the mesh at its authored real-world size
		CreateParticleShape(bunnyMesh, Vec3(0.0f, 0.0f, 0.0f), Vec3(maxEdge, maxEdge, maxEdge), 0.0f, spacing,
							Vec3(0.0f, 0.0f, 0.0f), invMass, true, 1.0f,
							NvFlexMakePhase(0, 0), true, 0.0f, Vec3(0.0f, 0.0f, 0.0f),
							0.0f, Vec4(0.6f, 0.55f, 0.45f, 1.0f));
		delete bunnyMesh;

		AddParticleShapeRigidFrames();
		PadRigidArrays();

		g_numSubsteps = numSubsteps;
		g_params.numIterations = numIterations;
		g_params.radius = spacing;
		g_params.dynamicFriction = dynamicFriction;
		g_params.dissipation = dissipation;
		g_params.sleepThreshold = spacing * sleepThresholdRatio;
		g_params.relaxationFactor = relaxationFactor;
		g_params.restitution = 0.0f;
		g_params.shapeCollisionMargin = shapeCollisionMargin;
		g_params.collisionDistance = collisionDistance;

		g_drawMesh = true;
		g_drawPoints = false;
		g_drawSprings = false;
	}
};
