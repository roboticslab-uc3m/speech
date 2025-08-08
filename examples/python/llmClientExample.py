#!/usr/bin/env python3

## @ingroup speech-examples-py
#  @defgroup llmClientExamplePy llmClientExample.py
#  @see @ref llmClientExample.py
## @example{lineno} llmClientExample.py

import argparse
import yarp

parser = argparse.ArgumentParser(description='LLM client example')
parser.add_argument('--remote', default='/llm_nws/rpc:i', type=str, help='remote port')

args = parser.parse_args()

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('Please start a YARP name server first')
    raise SystemExit

options = yarp.Property()
options.put('device', 'LLM_nwc_yarp')
options.put('local', '/llmClientExample/client')
options.put('remote', args.remote)

device = yarp.PolyDriver(options)

if not device.isValid():
    print('Remote device not available')
    raise SystemExit

llm = device.viewILLM()

if not llm:
    print('Unable to get ILLM interface')
    raise SystemExit

if not llm.setPrompt('You are a helpful mathematician.'):
    print('Failed to set prompt')
    raise SystemExit

prompt = yarp.SVector(1)

if not llm.readPrompt(prompt):
    print('Failed to read prompt')
    raise SystemExit
else:
    print(f'Prompt is: {prompt[0]}')

answer = yarp.LLM_Message()

question = 'What is 2 + 2?';
print(f'Asking: {question}')

if not llm.ask(question, answer):
    print('Failed to ask question')
    raise SystemExit

print('Answer:')
print(f'* type: {answer.type}')
print(f'* content: {answer.content}')
print(f'* parameters: {list(answer.parameters)}')
print(f'* arguments: {list(answer.arguments)}')

print('Deleting conversation')

if not llm.refreshConversation():
    print('Failed to refresh conversation')
    raise SystemExit

question = 'What is 3 + 3?'

print(f'Asking: {question}')

if not llm.ask(question, answer):
    print('Failed to ask question')
    raise SystemExit

print('Answer:')
print(f'* type: {answer.type}')
print(f'* content: {answer.content}')
print(f'* parameters: {list(answer.parameters)}')
print(f'* arguments: {list(answer.arguments)}')

conversation = yarp.LLMVector()

if not llm.getConversation(conversation):
    print('Failed to get conversation')
    raise SystemExit
else:
    print('Conversation:')

    for msg in conversation:
        print(msg.toString_c())

print('Deleting conversation and prompt')

if not llm.deleteConversation():
    print('Failed to delete conversation')
    raise SystemExit

if not llm.setPrompt('You are the worst mathematician ever, you always provide wrong answers.'):
    print('Failed to set prompt')
    raise SystemExit

if not llm.readPrompt(prompt):
    print('Failed to read prompt')
    raise SystemExit
else:
    print(f'Prompt is: {prompt[0]}')

question = 'What is 2 + 2?'

print(f'Asking: {question}')

if not llm.ask(question, answer):
    print('Failed to ask question')
    raise SystemExit

print('Answer:')
print(f'* type: {answer.type}')
print(f'* content: {answer.content}')
print(f'* parameters: {list(answer.parameters)}')
print(f'* arguments: {list(answer.arguments)}')

device.close()
