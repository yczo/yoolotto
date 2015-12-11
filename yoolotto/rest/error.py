import re
import datetime
import traceback

def format_error(request, exception):
    GET = {}
    POST = {}
    
    result = """

- - - - - - - - - - - - - - - - - - -
%s
    """ % datetime.datetime.now().isoformat()
    
    if request:
        GET = dict(request.GET)
        POST = dict(request.POST)
        META = dict(request.META)
        
        result += ("""
PATH: %s  ||  %s | %s
GET: %s
POST: %s
""") % (request.method, request.path, request.path_info, str(GET), str(POST))

        try:
            if request.body:
                result += "BODY: %s\n\n" % request.body
        except:
            pass
        
        _re = re.compile(r'^HTTP_.+$')
        for item in META:
            if _re.match(item):
                result += "%s: %s\n" % (item, META[item])
                
    result += "\n" + str(exception) + "\n" + str(exception.message) + "\n"
                
    result += "\n" + traceback.format_exc() + "\n\n^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^"
        
    return result