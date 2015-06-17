
// jconstants.h

#ifndef _JUDE_BASE_JCONSTANTS_H
#define _JUDE_BASE_JCONSTANTS_H


/*! \brief Provides various numerical and physical constants as static members. 

  Ultimately these may be located somehwere else.
*/

class JConstants {

public:

  /*! \brief Acceleration due to gravity in ms-2. */
  static const double gravity() { return 9.81; }

  /*! \brief Plank's constant times the speed of light. */
  static const double hc() { return 6.626068E-34 * 2.997925E8; }

  /*! \brief Avagadro's constant. */
  static const double avagadrosConstant() { return 6.022142E23; }

  /*! Constants from the Cox-Munk wind-speed law as reported by Mobley L&W. */
  static const double coxMunk_au_Mobley() { return 0.00316; }
  static const double coxMunk_ac_Mobley() { return 0.00192; }

  /*! Constants from the Cox-Munk wind-speed law as reported by Heron et al. 2006. */
  static const double coxMunk_au_Heron() { return 0.00312; }
  static const double coxMunk_ac_Heron() { return 0.00195; }
};



#endif // _JUDE_BASE_JCONSTANTS_H


