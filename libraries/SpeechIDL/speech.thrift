namespace yarp roboticslab

struct return_int_param {
    1: bool ret = false;
    2: i32 param = 0;
}

struct return_string_list {
    1: bool ret = false;
    2: list<string> string_list;
}

service TextToSpeechIDL
{
    /**
     * set the speech langauge
     * @return true/false on success/failure
     */
    bool setLanguage(1: string language);

    /**
     * set the speech speed
     * @return true/false on success/failure
     */
    bool setSpeed(1: i32 speed);

    /**
     * set the speech pitch
     * @return true/false on success/failure
     */
    bool setPitch(1: i32 pitch);

    /**
     * get the speech speed
     * @return struct {bool ret, i32 param}
     */
    return_int_param getSpeed();

    /**
     * get the speech pitch
     * @return struct {bool ret, i32 param}
     */
    return_int_param getPitch();

    /**
     * get the available languages
     * @return struct {bool ret, list<string> string_list}
     */
    return_string_list getSupportedLangs();

    /**
     * render and play the speech
     * @return true/false on success/failure
     */
    bool say(1: string text);

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

service SpeechRecognitionIDL
{
    /**
     * configure a dictionary
     * @param dictionary the dictionary to use, e.g. 'follow-me' or '20k'
     * @param language the language to use, e.g. 'en-us' or 'es'
     * @return true/false on success/failure
     */
    bool setDictionary(1: string dictionary, 2: string language);

    /**
     * mute the microphone
     * @return true/false on success/failure
     */
    bool muteMicrophone();

    /**
     * unmute the microphone
     * @return true/false on success/failure
     */
    bool unmuteMicrophone();
}
