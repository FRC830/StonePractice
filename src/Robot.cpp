/*practice code for 2016-2017 preseason
 *
 *
 */

//#include "WPILib.h"
#include "Lib830.h"
#include "Camera.h"
#include "RobotDrive.h"

//#include "../wpiutils/830utilities.h"
using namespace Lib830;

class Robot: public IterativeRobot
{

public:
	enum driverMode{ TANK_DRIVE = 1, REVERSE_TANK = 2, OPPO_ARCADE = 3, ARCADE_DRIVE = 4 }; //don't move drive is the default

private: 
	//drive train
	static const int LEFT_PWM_ONE = 0;
	static const int LEFT_PWM_TWO = 4;
	static const int RIGHT_PWM_ONE = 1;
	static const int RIGHT_PWM_TWO = 3;
	
	//sensors n' things
	static const int GYRO = 5;
	static const int ENCODER_A = 6;
	static const int ENCODER_B = 7;
	
	//handling robot burnout if we want to show motors other than drive train running  
	static const int INTAKE_PDP_CHANNEL = 11;
	Timer * timer;

	//controllers
	GamepadF310 * pilot;
	GamepadF310 * copilot;

	RobotDrive * drive;

	//more drive train
	VictorSP * frontLeft;
	VictorSP * backLeft;
	VictorSP * frontRight;
	VictorSP * backRight;

	//choosing between tank and arcade
	SendableChooser * modeChooser;
	driverMode driveMode;

	PowerDistributionPanel * pdp;
	
	//sensors!
	BuiltInAccelerometer * acceler;
	Lib830::AnalogGyro * gyro;
	Encoder * encoder;

	//cameras yay
	CAMERAFEEDS * cameraFeeds;

	//speed changer
	static const int TICKS_TO_FULL_SPEED = 15;

	void RobotInit() {

		//drive train!
		drive = new RobotDrive(
			new VictorSP(LEFT_PWM_ONE),
			new VictorSP(LEFT_PWM_TWO),
			new VictorSP(RIGHT_PWM_ONE),
			new VictorSP(RIGHT_PWM_TWO)
		);

		//controllers
		pilot = new GamepadF310(0);
		copilot = new GamepadF310(1);

		pdp = new PowerDistributionPanel();

		//putting the option to choose between the arcade and tank drives on the smart dashboard
		modeChooser = new SendableChooser();
		//modeChooser-> AddDefault("Arcade Drive", new driverMode(ARCADE_DRIVE));
		modeChooser-> AddObject("Tank Drive", new driverMode(TANK_DRIVE));
		modeChooser-> AddObject("Reverse Tank Drive", new driverMode(REVERSE_TANK));
		modeChooser-> AddObject("Opposite Arcade Drive", new driverMode(OPPO_ARCADE));
		modeChooser-> AddObject("Arcade Drive", new driverMode(ARCADE_DRIVE));
		SmartDashboard::PutData("Mode Chooser", modeChooser);
		
		//declaring all our sensors
		//gyro = new Lib830::AnalogGyro(GYRO);
		acceler = new BuiltInAccelerometer;
		encoder = new Encoder(ENCODER_A, ENCODER_B);
		timer = new Timer();

		//declaring camera stuff
		cameraFeeds = new CAMERAFEEDS;
		cameraFeeds->init();
	}

	void AutonomousInit()
	{

	}

	void AutonomousPeriodic()
	{

	}

	void TeleopInit()
	{

	}
	float previous_forward = 0.0;
	float leftforward, rightforward, targetForward, turn, forward;
	void TeleopPeriodic()
	{
		//switching between the different drive modes (Tank, Arcade)
		switch(driveMode) {
			
			case REVERSE_TANK:
				leftforward = accel(leftforward, pilot->LeftY(), TICKS_TO_FULL_SPEED);
				rightforward = accel(leftforward, pilot->RightX(), TICKS_TO_FULL_SPEED);
				drive->TankDrive(-leftforward,-rightforward,true);
				break;

			case TANK_DRIVE:
				leftforward = accel(leftforward, pilot->LeftY(), TICKS_TO_FULL_SPEED);
				rightforward = accel(leftforward, pilot->RightX(), TICKS_TO_FULL_SPEED);
				drive->TankDrive(leftforward,rightforward,true);
				break;

			case OPPO_ARCADE: 
				targetForward = pilot ->LeftY();
				turn = pilot->RightX()/1.4;
				forward = accel(previous_forward, targetForward, TICKS_TO_FULL_SPEED);
				drive->ArcadeDrive(-forward, turn, true);
				previous_forward = forward;
				break;

			case ARCADE_DRIVE:
				targetForward = pilot-> LeftY();
				turn = pilot->RightX()/-1.4;
				forward = accel(previous_forward, -targetForward, TICKS_TO_FULL_SPEED);
				if (forward > 0.5) {
					forward = 0.5;
				}
				else if (forward < -0.5) {
					forward = -0.5;
				}
				drive->ArcadeDrive(forward, turn, true);
				previous_forward = forward;
				break;

			default:
				drive-> ArcadeDrive(0,0,false);
			}

		//putting data on the smart dashboard
		//SmartDashboard::PutData("gyro", gyro);

		SmartDashboard::PutNumber("accelerometer Z", acceler->GetZ());

		SmartDashboard::PutNumber("Encoder", encoder->Get());

		//camera feed stuff - switching cameras
		//useless for now since we only have one camera on there...
		if (pilot->ButtonState(GamepadF310::BUTTON_X)) {
			cameraFeeds-> changeCam(cameraFeeds->kBtCamFront);
		}
		else if (pilot->ButtonState(GamepadF310::BUTTON_Y)){
			cameraFeeds-> changeCam(cameraFeeds->kBtCamBack);
		}

		cameraFeeds->run();

	}

	void DisabledInit() {
		cameraFeeds -> end();
	}
	
	void DisabledPeriodic() {
		driveMode = modeChooser->GetSelected() ? *(driverMode*)modeChooser->GetSelected() : ARCADE_DRIVE;

		//not quite sure about this, I think it's just a default 
	}
};

START_ROBOT_CLASS(Robot)
