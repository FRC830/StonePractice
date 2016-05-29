/*
 * Code For "Sword In The Stone" Robot
 */

#include "WPILib.h"
#include "Lib830.h"
#include "Clamp.h"

using namespace Lib830;

class Robot: public IterativeRobot{
private:

	//PWM pins
	static const int FRONT_LEFT_PWM = 1;
	static const int FRONT_RIGHT_PWM = 2;
	static const int BACK_LEFT_PWM = 3;
	static const int BACK_RIGHT_PWM = 4;

	static const int SWORD_CLAMP_PWM = 5;
	static const uint8 SWORD_CLAMP_CHANNEL = 13;
	static const int CLAMP_SWITCH_DIO = 1;
	static const int SWORD_SWITCH_DIO = 0;

	RobotDrive *drive;

	Clamp * clamp;

	GamepadF310 *pilot;

	PowerDistributionPanel * pdp;

	enum drive_mode_t { TANK_DRIVE, ARCADE_DRIVE };
	drive_mode_t drive_mode;
	// tank drive
	float left_speed, right_speed;
	// arcade drive
	float move_speed, rot_speed;
	static const int TICKS_TO_ACCEL = 50;
	SelectWidget<drive_mode_t> drive_mode_chooser;

	static constexpr float MOVE_SPEED_LIMIT = 0.6;

	void RobotInit()
	{
		SetDriveMode(ARCADE_DRIVE);

		drive = new RobotDrive(
				new Victor(FRONT_LEFT_PWM),
				new Victor(BACK_LEFT_PWM),
				new Victor(FRONT_RIGHT_PWM),
				new Victor(BACK_RIGHT_PWM)
		);
		clamp = new Clamp(
				new Victor(SWORD_CLAMP_PWM),
				new DigitalInput(CLAMP_SWITCH_DIO),
				new DigitalInput(SWORD_SWITCH_DIO)
		);

		pdp= new PowerDistributionPanel;

		pilot = new GamepadF310(0);

		drive_mode_chooser.AddOption("tank", TANK_DRIVE);
		drive_mode_chooser.AddOption("arcade", ARCADE_DRIVE, true);
		drive_mode_chooser.sendToDashboard("drive mode");

	}

	void AutonomousInit()
	{

	}

	void AutonomousPeriodic()
	{

	}

	void SetDriveMode(drive_mode_t new_mode)
	{
		drive_mode = new_mode;
		left_speed = 0;
		right_speed = 0;
		rot_speed = 0;
		move_speed = 0;
	}

	void TeleopInit()
	{
		puts("TeleopInit");
	}

	void TeleopPeriodic()
	{
		drive_mode_t new_mode = drive_mode_chooser.GetSelected();
		SmartDashboard::PutString("current mode", new_mode == TANK_DRIVE ? "Tank" : "Arcade");
		if (new_mode != drive_mode)
			SetDriveMode(new_mode);
		if (drive_mode == TANK_DRIVE) {
			left_speed = accel(left_speed, pilot->LeftY(), TICKS_TO_ACCEL);
			right_speed = accel(right_speed, pilot->RightY(), TICKS_TO_ACCEL);
			drive->TankDrive(left_speed, right_speed);
		}
		else {
			move_speed = accel(move_speed, pilot->LeftY(), TICKS_TO_ACCEL);
//			rot_speed = accel(rot_speed, pilot->RightX(), TICKS_TO_ACCEL);
//			SmartDashboard::PutNumber("rotation speed", rot_speed);
			rot_speed = pilot->RightX();
			drive->ArcadeDrive(move_speed * MOVE_SPEED_LIMIT, -rot_speed * MOVE_SPEED_LIMIT, false);
		}
		SmartDashboard::PutBoolean("clamp open", clamp->isOpen());
		SmartDashboard::PutBoolean("sword in", clamp->isSwordIn());
//		for (uint8 i = 0; i <= 15; ++i)
//			SmartDashboard::PutNumber(std::string("current #") + std::to_string(i), pdp->GetCurrent(i));
		SmartDashboard::PutNumber("Current", pdp->GetTotalCurrent());

		if (pilot->ButtonState(GamepadF310::BUTTON_A)) {
			clamp->open();
		}
		else if (pilot->ButtonState(GamepadF310::BUTTON_B)){
			clamp->close();
		}

		clamp->update();
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
