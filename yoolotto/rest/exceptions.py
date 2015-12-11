import json

from django.http import HttpResponse

class WebServiceException(Exception):
    status = 406
    visible = True
    verbose = "Unspecified Error"
    
    def response(self):
        result = {
            "type": self.__class__.__name__,
            "sucesss": False,
            "error": self.message if (self.visible and self.message) else self.verbose
        }
        
        return HttpResponse(json.dumps(result), status=self.status, 
            content_type="application/json")
    
class WebServiceAuthenticationFailed(WebServiceException):
    status = 401
    verbose = "Authentication Failed"
    
class WebServiceAuthorizationFailed(WebServiceException):
    status = 403
    verbose = "Authorization Failed"
    
class WebServiceObjectDoesNotExist(WebServiceException):
    status = 404
    verbose = "Object Does Not Exist"
    
class WebServiceFormValidationFailed(WebServiceException):
    status = 406
    verbose = "Form Validation Failed"
    
    def __init__(self, form):
        super(WebServiceFormValidationFailed, self).__init__(self.verbose) 
        self._form = form
        
    def response(self):
        result = {
            "type": self.__class__.__name__,
            "sucesss": False,
            "error": self._form.errors
        }
        
        return HttpResponse(json.dumps(result), status=self.status, 
            content_type="application/json")