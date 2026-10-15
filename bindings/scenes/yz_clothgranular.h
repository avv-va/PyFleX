class yz_ClothGranular: public yz_XArmTabletop
{
public:

	yz_ClothGranular(const char* name) : yz_XArmTabletop(name), mGranular(name), mCloth(name) {}

	yz_Granular mGranular;
	yz_Cloth mCloth;

	void Initialize(py::array_t<float> scene_params, int thread_idx = 0)
	{
		// granular params first, then cloth params; the cloth comes last so its solver settings are the scene's
		const int numGranularParams = 25;
		mGranular.Initialize(SceneParamsSlice(scene_params, 0, numGranularParams), thread_idx);
		mCloth.Initialize(SceneParamsSlice(scene_params, numGranularParams, scene_params.size() - numGranularParams), thread_idx);
		g_drawMesh = true;
	}
};
