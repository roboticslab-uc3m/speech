| Group | Parameter  |  Type  | Units | Default Value | Required |              Description               |        Notes         |
|:-----:|:----------:|:------:|:-----:|:-------------:|:--------:|:--------------------------------------:|:--------------------:|
|       |   model    | string |       |               |    no    |            model directory             |                      |
|       |    ngl     |  int   |       |      99       |    no    | number of layers to offload to the GPU |                      |
|       |   tokens   |  int   |       |      32       |    no    |      number of tokens to predict       |                      |
|       |   prompt   | string |       |               |    no    |      prompt to generate text from      | overrides promptFile |
|       | promptFile | string |       |               |    no    |       file containing the prompt       |                      |
