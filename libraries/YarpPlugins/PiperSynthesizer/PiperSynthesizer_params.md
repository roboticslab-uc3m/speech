| Group |     Parameter     |  Type  | Units | Default Value | Required |           Description            |               Notes               |
|:-----:|:-----------------:|:------:|:-----:|:-------------:|:--------:|:--------------------------------:|:---------------------------------:|
|       |     modelPath     | string |       |               |    no    |     path to .onnx voice file     |                                   |
|       |  modelConfigPath  | string |       |               |    no    |  path to JSON voice config file  |                                   |
|       |   eSpeakDataPath  | string |       |               |    no    | path to espeak-ng data directory |                                   |
|       |     speakerId     |  int   |       |       0       |    no    |          id of speaker           |                                   |
|       |     noiseScale    | float  |       |     0.667     |    no    |         generator noise          |                                   |
|       |    lengthScale    | float  |       |       1       |    no    |          phoneme length          | 1: normal, <1: faster, >1: slower |
|       |       noiseW      | float  |       |      0.8      |    no    |       phoneme width noise        |                                   |
