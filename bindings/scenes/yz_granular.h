class yz_Granular: public yz_XArmTabletop
{
public:

	yz_Granular(const char* name) : yz_XArmTabletop(name) {}

	void Initialize(py::array_t<float> scene_params, int thread_idx = 0)
	{
		auto ptr = (float *) scene_params.request().ptr;

		float radius = ptr[0];
		int numX = (int) ptr[1];
		int numY = (int) ptr[2];
		int numZ = (int) ptr[3];
		float granularScale = ptr[4];
		Vec3 lower = Vec3(ptr[5], ptr[6], ptr[7]);
		float granularDistance = ptr[8];
		float shapeCollisionMargin = ptr[9];
		float collisionDistance = ptr[10];
		float dynamicFriction = ptr[11];
		float mass = ptr[12];
		bool regularShape = ptr[13] != 0.0f;
		float shapeMinDistance = ptr[14];
		float shapeMaxDistance = ptr[15];
		int minPlanes = (int) ptr[16];
		int maxPlanes = (int) ptr[17];
		float rigidStiffness = ptr[18];
		int numSubsteps = (int) ptr[19];
		int numIterations = (int) ptr[20];
		float dissipation = ptr[21];
		float sleepThresholdRatio = ptr[22];
		float relaxationFactor = ptr[23];
		uint32_t seed = (uint32_t) ptr[24];

		seed1 = seed;
		seed2 = seed1 ^ 0x13ab45fe;

		int group = 1;  // group 0 is left to the rope or cloth sharing the scene
		float pitch = granularScale + granularDistance;
		for (int x = 0; x < numX; x++)
		{
			for (int z = 0; z < numZ; z++)
			{
				for (int y = 0; y < numY; y++)
				{
					Mesh* piece = CreateRandomConvexMesh(Rand(minPlanes, maxPlanes), shapeMinDistance, shapeMaxDistance, regularShape);
					CreateParticleShape(piece, lower + pitch*Vec3(float(x), float(y), float(z)),
										granularScale, 0.0f, radius*1.001f,
										0.0f, 1.0f/mass, true, rigidStiffness,
										NvFlexMakePhase(group++, 0), true, radius*0.1f, 0.0f,
										0.0f, Vec4(237.0f/255.0f, 145.0f/255.0f, 33.0f/255.0f, 1.0f));
					delete piece;
				}
			}
		}

		g_numSubsteps = numSubsteps;

		g_params.numIterations = numIterations;
		g_params.radius = radius;
		g_params.dynamicFriction = dynamicFriction;
		g_params.dissipation = dissipation;
		g_params.sleepThreshold = radius*sleepThresholdRatio;
		g_params.relaxationFactor = relaxationFactor;
		g_params.restitution = 0.0f;
		g_params.shapeCollisionMargin = shapeCollisionMargin;
		g_params.collisionDistance = collisionDistance;

		g_drawMesh = true;
		g_drawPoints = false;
		g_drawSprings = false;
	}
};
