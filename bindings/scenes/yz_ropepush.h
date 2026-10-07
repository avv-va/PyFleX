class yz_RopePush: public Scene
{
public:

	yz_RopePush(const char* name) : Scene(name) {}

	void Initialize(py::array_t<float> scene_params, int thread_idx = 0)
	{
		auto ptr = (float *) scene_params.request().ptr;

		float radius = ptr[0];
		Vec3 ropeStart = Vec3(ptr[1], ptr[2], ptr[3]);
		Vec3 ropeDir = Normalize(Vec3(ptr[4], ptr[5], ptr[6]));
		float ropeLength = ptr[7];
		int ropeSegments = (int) ptr[8];
		float ropeStiffness = ptr[9];
		float pusherRadius = ptr[10];
		float pusherHalfHeight = ptr[11];
		Vec3 pusherPosition = Vec3(ptr[12], ptr[13], ptr[14]);
		float friction = ptr[15];
		float dt = ptr[16];
		int numSubsteps = (int) ptr[17];
		int numIterations = (int) ptr[18];
		float damping = ptr[19];
		float dissipation = ptr[20];

		Rope rope;
		CreateRope(rope, ropeStart, ropeDir, ropeStiffness, ropeSegments, ropeLength, NvFlexMakePhase(0, eNvFlexPhaseSelfCollide));
		g_ropes.push_back(rope);

		AddCapsule(pusherRadius, pusherHalfHeight, pusherPosition, QuatFromAxisAngle(Vec3(0.0f, 0.0f, 1.0f), kPi * 0.5f));

		g_params.radius = radius;
		g_params.numIterations = numIterations;
		g_params.dynamicFriction = friction;
		g_params.staticFriction = friction;
		g_params.particleFriction = friction;
		g_params.damping = damping;
		g_params.dissipation = dissipation;

		g_dt = dt;
		g_numSubsteps = numSubsteps;

		g_drawPoints = true;
		g_drawMesh = false;
		g_drawEllipsoids = false;
		g_drawDiffuse = false;
		g_drawRopes = true;
	}

	virtual void CenterCamera()
	{
		g_camPos = Vec3(0.0f, 1.6f, 2.2f);
		g_camAngle = Vec3(0.0f, -DegToRad(36.0f), 0.0f);
	}
};
