import re
import json

class GenericFormatter(object):
    NAME = "Generic"
    
    @staticmethod
    def decode(raw):
        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
                
        return json.dumps(result)
        
    @staticmethod
    def representation(encoded):
        values = json.loads(encoded)
        return "-".join(values[:-1])