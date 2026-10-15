// a combined scene hands each object its own slice of scene_params
py::array_t<float> SceneParamsSlice(py::array_t<float> scene_params, int start, int count)
{
	return py::array_t<float>(count, (float *) scene_params.request().ptr + start);
}

// CreateParticleShape rigids get their centres of mass only at the end of Init, and only if no rigid has one yet;
// compute them now so a rope (whose clusters bring their own) can follow them in the rigid arrays
void AddParticleShapeRigidFrames()
{
	const int numRigids = g_buffers->rigidOffsets.size() - 1;
	g_buffers->rigidTranslations.resize(numRigids, Vec3());
	CalculateRigidCentersOfMass(&g_buffers->positions[0], g_buffers->positions.size(), &g_buffers->rigidOffsets[0], &g_buffers->rigidTranslations[0], &g_buffers->rigidIndices[0], numRigids);
	g_buffers->rigidRotations.resize(numRigids, Quat());
}

// the rope's clusters bring no local normals or skinned mesh vertices, unlike CreateParticleShape rigids; pad them so the arrays line up
// (the viewer's SkinMesh reads rigidMeshSize for every rigid)
void PadRigidArrays()
{
	g_buffers->rigidLocalNormals.resize(g_buffers->rigidIndices.size(), Vec4(0.0f));
	g_buffers->rigidMeshSize.resize(g_buffers->rigidRotations.size(), 0);
}

class yz_XArmTabletop: public Scene
{
public:

	yz_XArmTabletop(const char* name) : Scene(name) {}

	virtual void CenterCamera()
	{
		g_camPos = Vec3(0.75f, 0.9f, 0.0f);
		g_camAngle = Vec3(DegToRad(90.0f), -DegToRad(50.0f), 0.0f);
	}
};
