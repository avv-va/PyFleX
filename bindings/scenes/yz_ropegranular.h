class yz_RopeGranular: public yz_SoftRope
{
public:

	yz_RopeGranular(const char* name) : yz_SoftRope(name), mGranular(name) {}

	yz_Granular mGranular;

	void Initialize(py::array_t<float> scene_params, int thread_idx = 0)
	{
		// granular params first, then rope params; the rope comes last so its solver settings are the scene's
		const int numGranularParams = 25;
		mGranular.Initialize(SceneParamsSlice(scene_params, 0, numGranularParams), thread_idx);
		AddParticleShapeRigidFrames();
		yz_SoftRope::Initialize(SceneParamsSlice(scene_params, numGranularParams, scene_params.size() - numGranularParams), thread_idx);
		PadRigidArrays();
	}
};
