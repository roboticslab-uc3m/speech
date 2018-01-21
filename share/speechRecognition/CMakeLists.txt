# Copyright: 2016 UC3M
# Authors: Juan G Victores & Raul de Santos Rico
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_install(FILES conf/speechRecognition.ini
             DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR}/speechRecognition)

yarp_install(FILES conf/dictionary/es-20k.dic
                   conf/dictionary/es-20k.lm
                   conf/dictionary/follow-me-english.dic
                   conf/dictionary/follow-me-english.lm
                   conf/dictionary/follow-me-spanish.dic
                   conf/dictionary/follow-me-spanish.lm
                   conf/dictionary/waiter-english.dic
                   conf/dictionary/waiter-english.lm
                   conf/dictionary/waiter-spanish.dic
                   conf/dictionary/waiter-spanish.lm
             DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR}/speechRecognition/dictionary)

yarp_install(FILES conf/model/en-us/feat.params
                   conf/model/en-us/feature_transform
                   conf/model/en-us/mdef
                   conf/model/en-us/means
                   conf/model/en-us/mixture_weights
                   conf/model/en-us/noisedict
                   conf/model/en-us/README
                   conf/model/en-us/transition_matrices
                   conf/model/en-us/variances
             DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR}/speechRecognition/model/en-us)

yarp_install(FILES conf/model/es/feat.params
                   conf/model/es/mdef
                   conf/model/es/means
                   conf/model/es/mixture_weights
                   conf/model/es/noisedict
                   conf/model/es/sendump
                   conf/model/es/transition_matrices
                   conf/model/es/variances
             DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR}/speechRecognition/model/es)
