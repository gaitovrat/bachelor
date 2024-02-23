#ifndef _DEFINES_H
#define _DEFINES_H

#define COLOR_BLACK 0x00
#define COLOR_WHITE 0xff
#define COLOR_WHITE_ORIGINAL 0xfff
#define BLACK_COUNT 10
#define CONST_ERROR 160.6f
#define CONST_DERIVATIVE 8.3f
#define CONST_INTEGRAL 0.5f
#define TURN_CONTROL_COEFICIENT 1.28f
#define SEND_REGIONS_NUM 15
#define LOW_DIVERSITY 100
#define PREV_LINE_SEARCH_REGION 5
#define MAX_REGIONS_COUNT 25

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define bit(b) (1UL << (b))

#define TFC_VERSION "20171213"

#define __TFC_EMBEDED__ 1

/** Resolution of the camera. */
#define TFC_CAMERA_LINE_LENGTH 128

/** Specifies the integer range for PWM, as in <-::TFC_PWM_MINMAX,
   ::TFC_PWM_MINMAX>, which is used in this application's methods
        TFC::getMotorPWM_i() and TFC::setMotorPWM_i().
*/
#define TFC_PWM_MINMAX 1000

/** Specifies the integer range for servo, as in <-::TFC_SERVO_MINMAX,
   ::TFC_SERVO_MINMAX>, which is used in this application's methods
        TFC::getServo_i() and TFC::setServo_i().
*/
#define TFC_SERVO_MINMAX 1000

/** Maximum value of analog sample obtained from ADC by the method
 * TFC::ReadADC(). */
#define TFC_ADC_MAXVAL 0x0FFF

/** Specifies the integer ranges for analog values, as in <-::TFC_ANDATA_MINMAX,
   ::TFC_ANDATA_MINMAX> or <0, ::TFC_ANDATA_MINMAX>, which are used in this
   application's methods TFC::ReadPot_i(), TFC::ReadFB_i() and
   TFC::ReadBatteryVoltage_i().
*/
#define TFC_ANDATA_MINMAX 1000

/** Default center position of servos - pulse width in microseconds. */
#define TFC_SERVO_DEFAULT_CENTER 1500

/** Specifies the default offset from ::TFC_SERVO_DEFAULT_CENTER, which
 * corresponds to fully turned wheels to either side. */
#define TFC_SERVO_DEFAULT_MAX_LR 200

/** Specifies the maximal offset from the servos' center, which can be used in
 * calibration by method TFC::setServoCalibration(). */
#define TFC_SERVO_MAX_LR 400

/** Specifies the default integer range for PWM, as in <-::TFC_PWM_DEFAULT_MAX,
   ::TFC_PWM_DEFAULT_MAX>, which can be changed in the method TFC::setPWMMax()
   up to <-::TFC_PWM_MINMAX, ::TFC_PWM_MINMAX>.
*/
#define TFC_PWM_DEFAULT_MAX 200
#define CONTROL_PWM_MAX 300

/** Maximal allowed -/+ PWM duty cycle for the underlying HW. */
#define HW_TFC_PWM_MAX 1000

/** Minimal allowed length of pulse for steering servo for the underlying HW */
#define HW_TFC_SERVO_MIN 500
/** Maximal allowed length of pulse for steering servo  for the underlying HW*/
#define HW_TFC_SERVO_MAX 2500

/** Command used in a packet for sending/receiving data. */
#define CMD_DATA 1

/** Command used in a packet for calibration. */
#define CMD_SETTING 2

/** Command used in a packet for controlling features of the board or receiving
 * data from them. */
#define CMD_CONTROL 3

/** Packet header. */
#define STX 0x2
/** Packet footer. */
#define ETX 0x3

enum tfc_andata_chnl_enum {
  anPOT_1, ///< Potentiometer 1.
  anPOT_2, ///< Potentiometer 2.
  anFB_A,  ///< Feedback A from the H-Bridge.
  anFB_B,  ///< Feedback B from the H-Bridge.
  anBAT,   ///< Battery voltage.
  anIR_1,  ///< IR sensor 1
  anIR_2,  ///< IR sensor 2
  anIR_3,  ///< IR sensor 3
  anLast   ///< Last member used as a size of an array.
};

#endif //_DEFINES_H
