class yz_RopeBox: public yz_SoftRope
{
public:

	yz_RopeBox(const char* name) : yz_SoftRope(name) {}

	void Initialize(py::array_t<float> scene_params, int thread_idx = 0)
	{
		auto ptr = (float *) scene_params.request().ptr;

		// box params first, then rope params; the rope comes last so its solver settings are the scene's
		Vec3 lower = Vec3(ptr[0], ptr[1], ptr[2]);
		Vec3 size = Vec3(ptr[3], ptr[4], ptr[5]);
		float spacing = ptr[6];
		float invMass = ptr[7];
		const int numBoxParams = 8;

		char path[200];
		strcpy(path, getenv("PYFLEXROOT"));
		strcat(path, "/data/box.ply");

		CreateParticleShape(GetFilePathByPlatform(path).c_str(), lower, size, 0.0f, spacing,
							0.0f, invMass, true, 1.0f,
							NvFlexMakePhase(1, 0), true, 0.0f, 0.0f,
							0.0f, Vec4(0.2f, 0.6f, 0.9f, 1.0f));

		AddParticleShapeRigidFrames();
		yz_SoftRope::Initialize(SceneParamsSlice(scene_params, numBoxParams, scene_params.size() - numBoxParams), thread_idx);
		PadRigidArrays();
	}
};
