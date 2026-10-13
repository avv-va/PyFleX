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
