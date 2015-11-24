import json

def dumps(value):
    return json.dumps(value, sort_keys=True, separators=(',', ':'))