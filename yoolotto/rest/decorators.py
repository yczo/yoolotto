import time
import json
import logging
import traceback
import hashlib

from django.core.exceptions import ObjectDoesNotExist
from django.core.handlers.wsgi import WSGIRequest
from django.conf import settings
from django.db import IntegrityError
from django.http import HttpResponse


from yoolotto.rest.exceptions import WebServiceException, WebServiceObjectDoesNotExist, \
    WebServiceAuthenticationFailed
from yoolotto.rest.error import format_error

from yoolotto.user.models import Device, YooLottoUser
from yoolotto.coin.models import DeviceCoins

class Authenticate(object):
    
    INVALID_IOS7_DEVICE_ID = "fd31e787d5d187c079c924883431093a"
            
    def __init__(self, create=True):
        self._create = create
        
    def __call__(self, fn):
        def wrapped(*args, **kwargs):
            request = args[1]
            if request.META["HTTP_YOO_APP_VERSION"] >= "5.1":
                
                if not isinstance(request, WSGIRequest):
                    raise RuntimeError("Invalid Request Object")
                
                if "HTTP_YOO_DEVICE_ID" not in request.META:
                    raise WebServiceAuthenticationFailed()
                hashstring = hashlib.sha1("@#!" + request.META['HTTP_YOO_EMAIL_ID'] + "yoolottospa").hexdigest()
                #print type(request.META["HTTP_YOO_AUTH_TOKEN"])
                #print type(hashstring)
                if hashstring != request.META["HTTP_YOO_AUTH_TOKEN"]:
                    raise WebServiceAuthenticationFailed()
                required_user = YooLottoUser.objects.get(email = request.META['HTTP_YOO_EMAIL_ID'])
                request.yoo["new_version"] = False
                request.yoo["user"] = required_user
                enum_old_version = {"IPHONE":3.0,"ANDROID":3.6}
                old_version = enum_old_version[request.META["HTTP_YOO_DEVICE_TYPE"]]
                
                if str(request.META["HTTP_YOO_APP_VERSION"]) > str(old_version):
                    request.yoo["new_version"] = True
                print "the user is ", request.yoo["user"].id        
                try:
                    device, created = Device.objects.get_or_create(device_id=request.META["HTTP_YOO_DEVICE_ID"], user = request.yoo["user"])                
                except IntegrityError:
                    time.sleep(0.05)
                    device = Device.objects.get(device_id=request.META["HTTP_YOO_DEVICE_ID"], user = request.yoo["user"])

                device.device_type = request.META.get("HTTP_YOO_DEVICE_TYPE", None)
                device.app_version = request.META.get("HTTP_YOO_APP_VERSION", None)
                device.os_version = request.META.get("HTTP_YOO_OS_VERSION", None)
                
                if not device.device_type:
                    device.device_type = "UNKNOWN"
                        
                request.yoo["device"] = device
                request.yoo["auth"]["success"] = True
                device.save()
                            
                return fn(*args, **kwargs)
            else:
                if not isinstance(request, WSGIRequest):
                    raise RuntimeError("Invalid Request Object")
                
                if "HTTP_YOO_DEVICE_ID" not in request.META:
                    raise WebServiceAuthenticationFailed()
                
                if request.META["HTTP_YOO_DEVICE_ID"] == self.INVALID_IOS7_DEVICE_ID:
                    request.yoo["device"] = None
                    request.yoo["user"] = None
                    request.yoo["auth"]["success"] = False
                    request.yoo["auth"]["__internal__reject__"] = True
                    
                    return fn(*args, **kwargs)

                request.yoo["new_version"] = False

                enum_old_version = {"IPHONE":3.0,"ANDROID":3.6}
                old_version = enum_old_version[request.META["HTTP_YOO_DEVICE_TYPE"]]
                
                if str(request.META["HTTP_YOO_APP_VERSION"]) > str(old_version):
                    request.yoo["new_version"] = True
                        
                try:
                    device = Device.objects.filter(device_id=request.META["HTTP_YOO_DEVICE_ID"])[0]               
                except:
                    if not self._create:
                        raise WebServiceAuthenticationFailed()
                    else:
                        user = YooLottoUser()
                        user.save()
                        
                        device = Device(user=user, device_id=request.META["HTTP_YOO_DEVICE_ID"])
                
                device.device_type = request.META.get("HTTP_YOO_DEVICE_TYPE", None)
                device.app_version = request.META.get("HTTP_YOO_APP_VERSION", None)
                device.os_version = request.META.get("HTTP_YOO_OS_VERSION", None)
                
                if not device.device_type:
                    device.device_type = "UNKNOWN"
                
                try:
                    device.save()
                except IntegrityError:
                    try:
                        time.sleep(0.05)
                        _device = Device.objects.get(device_id=request.META["HTTP_YOO_DEVICE_ID"])
                    except Device.DoesNotExist:
                        device.save()
                    else:
                        device = _device
                        
                request.yoo["device"] = device
                request.yoo["user"] = device.user
                request.yoo["auth"]["success"] = True
                            
                return fn(*args, **kwargs)

        return wrapped

logger = logging.getLogger("yoolotto.uncaught")


def rest(fn):
    def wrapped(*args, **kwargs):
        try:
            request = args[1]
            
            if not hasattr(request, "yoo"):
                request.yoo = {
                    "user": None,
                    "auth": {
                        "success": False,
                        "required": False
                    }
                }
            
            result = fn(*args, **kwargs)
        except WebServiceException as e:
            return e.response()
        except ObjectDoesNotExist as e:
            logger.error(e)
            logger.error(traceback.format_exc())
            if settings.DEBUG:
                _e = WebServiceObjectDoesNotExist(e.message)
            else:
                _e = WebServiceObjectDoesNotExist("ObjectDoesNotExist")
            return _e.response()
        except Exception as e:
            _err = ""
            
            try:
                _err = format_error(request, e)
            except:
                _err = str(e)
            
            try:    
                logger.error(_err)
            except:
                _err += "\n !COULD NOT LOG EXCEPTION LOCALLY!"
            
            from yoolotto.lottery.game.base import LotteryPlayInvalidException
            
            if settings.ERROR_NOTIFICATION and not isinstance(e, LotteryPlayInvalidException):
                try:
                    from yoolotto.communication.email import EmailSender
                    email = EmailSender(settings.ERROR_EMAIL, "YL Exception: " + str(e), 
                        text=_err)
                    email.send()
                except:
                    logger.error("\n\nERROR HANDLING EXCEPTION")
                    logger.error(traceback.format_exc() + "\n\n")
            
            if hasattr(e, "representation"):
                return e.representation()
            
            raise
        
        if not isinstance(result, HttpResponse):
            result = json.dumps(result)
            result = HttpResponse(result, content_type="application/json")
        return result
    return wrapped
