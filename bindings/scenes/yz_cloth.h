class yz_Cloth: public yz_XArmTabletop
{
public:

	yz_Cloth(const char* name) : yz_XArmTabletop(name) {}

	void Initialize(py::array_t<float> scene_params, int thread_idx = 0)
	{
		auto ptr = (float *) scene_params.request().ptr;

		Vec3 lower = Vec3(ptr[0], ptr[1], ptr[2]);
		int dimX = (int) ptr[3];
		int dimZ = (int) ptr[4];
		float stretchStiffness = ptr[5];
		float bendStiffness = ptr[6];
		float shearStiffness = ptr[7];
		float totalMass = ptr[8];
		float radius = ptr[9];
		float dynamicFriction = ptr[10];
		float staticFriction = ptr[11];
		float particleFriction = ptr[12];
		int numSubsteps = (int) ptr[13];
		int numIterations = (int) ptr[14];
		float radiusExpansion = ptr[15];
		float damping = ptr[16];
		float sleepThreshold = ptr[17];
		float relaxationFactor = ptr[18];
		float shapeCollisionMargin = ptr[19];
		float collisionDistance = ptr[20];

		int phase = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseSelfCollideFilter);
		float mass = totalMass / (dimX * dimZ);
		CreateSpringGrid(lower, dimX, dimZ, 1, radius, phase, stretchStiffness, bendStiffness, shearStiffness, 0.0f, 1.0f / mass);

		g_params.dynamicFriction = dynamicFriction;
		g_params.staticFriction = staticFriction;
		g_params.particleFriction = particleFriction;

		g_numSubsteps = numSubsteps;
		g_params.numIterations = numIterations;
		g_params.radius = radius * radiusExpansion;

		g_params.damping = damping;
		g_params.sleepThreshold = sleepThreshold;
		g_params.relaxationFactor = relaxationFactor;

		g_params.shapeCollisionMargin = shapeCollisionMargin;
		g_params.collisionDistance = collisionDistance;

		g_drawPoints = false;
		g_drawCloth = true;
		g_drawSprings = false;
	}
};
