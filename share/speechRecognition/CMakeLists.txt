# Copyright: 2016 UC3M
# Authors: Juan G Victores & Raul de Santos Rico
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_install(FILES speechRecognition.ini
             DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR}/speechRecognition)

yarp_install(FILES dictionary/es-20k.dic
                   dictionary/es-20k.lm
                   dictionary/follow-me-english.dic
                   dictionary/follow-me-english.lm
                   dictionary/follow-me-spanish.dic
                   dictionary/follow-me-spanish.lm
                   dictionary/waiter-english.dic
                   dictionary/waiter-english.lm
                   dictionary/waiter-spanish.dic
                   dictionary/waiter-spanish.lm
             DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR}/speechRecognition/dictionary)

yarp_install(FILES model/en-us/feat.params
                   model/en-us/feature_transform
                   model/en-us/mdef
                   model/en-us/means
                   model/en-us/mixture_weights
                   model/en-us/noisedict
                   model/en-us/README
                   model/en-us/transition_matrices
                   model/en-us/variances
             DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR}/speechRecognition/model/en-us)

yarp_install(FILES model/es/feat.params
                   model/es/mdef
                   model/es/means
                   model/es/mixture_weights
                   model/es/noisedict
                   model/es/sendump
                   model/es/transition_matrices
                   model/es/variances
             DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR}/speechRecognition/model/es)
