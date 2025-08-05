| Group |   Parameter   |  Type  | Units | Default Value | Required |       Description        |               Notes               |
|:-----:|:-------------:|:------:|:-----:|:-------------:|:--------:|:------------------------:|:---------------------------------:|
|       |     model     | string |       |               |    no    |     ONNX voice file      |                                   |
|       |  modelConfig  | string |       |               |    no    |  JSON voice config file  |                                   |
|       | eSpeakDataDir | string |       |               |    no    | espeak-ng data directory |                                   |
|       |   speakerId   |  int   |       |       0       |    no    |      id of speaker       |                                   |
|       |   noiseScale  | float  |       |     0.667     |    no    |     generator noise      |                                   |
|       |  lengthScale  | float  |       |       1       |    no    |      phoneme length      | 1: normal, <1: faster, >1: slower |
|       |     noiseW    | float  |       |      0.8      |    no    |   phoneme width noise    |                                   |
