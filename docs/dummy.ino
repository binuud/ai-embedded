#include <Arduino.h>

// --- Pin Definitions ---
#define MOTOR_L_PWM  25
#define MOTOR_L_DIR  26
#define MOTOR_R_PWM  27
#define MOTOR_R_DIR  14

#define ENCODER_L_A  32
#define ENCODER_L_B  33
#define ENCODER_R_A  34
#define ENCODER_R_B  35

// --- Robot Physical Parameters ---
const float WHEEL_DIAMETER = 0.065; // meters
const float WHEEL_BASE     = 0.150; // meters (distance between wheels)
const int   CPR            = 360;   // Encoder Counts Per Revolution
const float METERS_PER_TICK = (PI * WHEEL_DIAMETER) / CPR;

// --- Robot State (Odometry) ---
volatile long left_ticks = 0;
volatile long right_ticks = 0;
float robot_x = 0.0, robot_y = 0.0, robot_theta = 0.0;

// --- Target Pose ---
float target_x = 1.0, target_y = 1.0, target_theta = 0.0; // 1m, 1m, 0 rad

// --- PID Gains & Structs ---
struct PIDController {
    float kp, ki, kd;
    float integral, last_error;
    float limit;
};

// Inner speed loop gains
PIDController pid_left  = {2.5, 0.5, 0.1, 0.0, 0.0, 255.0};
PIDController pid_right = {2.5, 0.5, 0.1, 0.0, 0.0, 255.0};

// Outer pose loop gains
const float K_RHO   = 1.2;  // Linear velocity proportional gain
const float K_ALPHA = 2.5;  // Heading proportional gain
const float K_PSI   = 1.5;  // Final angle correction gain

// --- ISR Encoder Functions ---
void IRAM_ATTR left_encoder_isr() {
    if (digitalRead(ENCODER_L_A) == digitalRead(ENCODER_L_B)) left_ticks++;
    else left_ticks--;
}
void IRAM_ATTR right_encoder_isr() {
    if (digitalRead(ENCODER_R_A) == digitalRead(ENCODER_R_B)) right_ticks++;
    else right_ticks--;
}

// --- Helper Functions ---
float normalize_angle(float angle) {
    return atan2(sin(angle), cos(angle));
}

float update_pid(PIDController &pid, float error, float dt) {
    pid.integral += error * dt;
    pid.integral = constrain(pid.integral, -pid.limit, pid.limit); // Prevent windup
    float derivative = (error - pid.last_error) / dt;
    pid.last_error = error;
    float output = (pid.kp * error) + (pid.ki * pid.integral) + (pid.kd * derivative);
    return constrain(output, -pid.limit, pid.limit);
}

void set_motor_speeds(int pwm_l, int pwm_r) {
    digitalWrite(MOTOR_L_DIR, pwm_l >= 0 ? HIGH : LOW);
    digitalWrite(MOTOR_R_DIR, pwm_r >= 0 ? HIGH : LOW);
    analogWrite(MOTOR_L_PWM, abs(pwm_l));
    analogWrite(MOTOR_R_PWM, abs(pwm_r));
}

// --- Core Controls Tasks ---
void control_loop(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20); // 50 Hz loop
    
    long last_left_ticks = 0, last_right_ticks = 0;
    float dt = 0.02;

    while (true) {
        // 1. Snapshot encoders atomically
        noInterrupts();
        long curr_left = left_ticks;
        long curr_right = right_ticks;
        interrupts();

        long d_left = curr_left - last_left_ticks;
        long d_right = curr_right - last_right_ticks;
        last_left_ticks = curr_left;
        last_right_ticks = curr_right;

        // 2. Dead Reckoning (Odometry Update)
        float d_s_left = d_left * METERS_PER_TICK;
        float d_s_right = d_right * METERS_PER_TICK;
        float d_center = (d_s_left + d_s_right) / 2.0;
        float d_theta = (d_s_right - d_s_left) / WHEEL_BASE;

        robot_x += d_center * cos(robot_theta + d_theta / 2.0);
        robot_y += d_center * sin(robot_theta + d_theta / 2.0);
        robot_theta = normalize_angle(robot_theta + d_theta);

        // Live measured velocities
        float measured_v_l = d_s_left / dt;
        float measured_v_r = d_s_right / dt;

        // 3. Outer Loop: Pose Kinematics Control
        float dx = target_x - robot_x;
        float dy = target_y - robot_y;
        float rho = sqrt(dx*dx + dy*dy);
        
        float target_v = 0.0;
        float target_w = 0.0;

        if (rho > 0.03) { // Threshold: 3cm tolerance
            // Move toward target position
            float alpha = normalize_angle(atan2(dy, dx) - robot_theta);
            
            // Backwards motion adjustment if target is behind the robot
            if (alpha > PI/2 || alpha < -PI/2) {
                alpha = normalize_angle(alpha + PI);
                target_v = -K_RHO * rho;
            } else {
                target_v = K_RHO * rho;
            }
            target_w = K_ALPHA * alpha;
        } else {
            // Arrived at position: correct final angle orientation
            float psi = normalize_angle(target_theta - robot_theta);
            if (abs(psi) > 0.05) { // 3 degrees tolerance
                target_v = 0.0;
                target_w = K_PSI * psi;
            } else {
                // Goal completely achieved
                target_v = 0.0;
                target_w = 0.0;
            }
        }

        // Limit maximum velocities
        target_v = constrain(target_v, -0.4, 0.4); // max 0.4 m/s
        target_w = constrain(target_w, -1.5, 1.5); // max 1.5 rad/s

        // 4. Inverse Kinematics (Target linear/angular -> Target Wheel speeds)
        float target_v_l = target_v - (target_w * WHEEL_BASE / 2.0);
        float target_v_r = target_v + (target_w * WHEEL_BASE / 2.0);

        // 5. Inner Loop: Velocity PID to PWM
        float err_l = target_v_l - measured_v_l;
        float err_r = target_v_r - measured_v_r;

        int pwm_output_l = (int)update_pid(pid_left, err_l, dt);
        int pwm_output_r = (int)update_pid(pid_right, err_r, dt);

        set_motor_speeds(pwm_output_l, pwm_output_r);

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(MOTOR_L_PWM, OUTPUT);
    pinMode(MOTOR_L_DIR, OUTPUT);
    pinMode(MOTOR_R_PWM, OUTPUT);
    pinMode(MOTOR_R_DIR, OUTPUT);

    pinMode(ENCODER_L_A, INPUT_PULLUP);
    pinMode(ENCODER_L_B, INPUT_PULLUP);
    pinMode(ENCODER_R_A, INPUT_PULLUP);
    pinMode(ENCODER_R_B, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENCODER_L_A), left_encoder_isr, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_R_A), right_encoder_isr, RISING);

    // Pinpoint execution time precisely by pinning control system to Core 1
    xTaskCreatePinnedToCore(control_loop, "ControlLoop", 4096, NULL, 3, NULL, 1);
}

void loop() {
    // Core 0 executes asynchronously to output telemetry without delaying control calculations
    Serial.printf("X: %.2f | Y: %.2f | Theta: %.2f | Left Ticks: %ld | Right Ticks: %ld\n", 
                  robot_x, robot_y, robot_theta, left_ticks, right_ticks);
    delay(100);
}