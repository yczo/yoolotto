# import re
# import json

# class SuperLottoPlusFormatter(object):
#     NAME = "SuperLottoPlus"
    
#     @staticmethod
#     def decode(raw):
#         pattern = re.compile(r"\d+")
#         result = map(int, re.findall(pattern, raw))
        
#         if len(result) != 6:
#             raise ValueError("Unexpected Decode Value: %s" % result)
        
#         return json.dumps(result)
        
#     @staticmethod
#     def representation(encoded):
#         values = json.loads(encoded)
        
#         if len(values) != 6:
#             raise ValueError("Unexpected Representation Value: %s" % result)
        
#         return "-".join(values[:-1]) + " Superlotto: " + values[-1]