# Copyright: (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Ali Paikan
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
#
# speech.thrift

/**
* Speech_IDL
*/

namespace yarp roboticslab

service Speech_IDL
{

  /**
  * set the speech langauge
  * @return true/false on success/failure
  */
  bool setLanguage(string language);

  /**
  * set the speech speed
  * @return true/false on success/failure
  */
  bool setSpeed(i16 speed);

  /**
  * set the speech pitch
  * @return true/false on success/failure
  */
  bool setPitch(i16 pitch);

  /**
  * get the speech speed
  * @return the speed
  */
  i16 getSpeed();

  /**
  * get the speech pitch
  * @return the pitch
  */
  i16 getPitch();

  /**
  * get the available languages
  * @return a list of the supported languages
  */
  list<string> getSupportedLangs();

  /**
  * render and play the speech
  * @return true/false on success/failure
  */
  bool say(string text);

 /**
  * play the previously rendered or paused speech
  */
  bool play();

 /**
  * pause the current speech
  */
  bool pause();

  /**
  * stop playing the current speech
  */
  bool stop();

  /**
   * check whether there is an ongoing speech right now
   */
  bool checkSayDone();

}
