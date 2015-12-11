import datetime
import math
import json
from django.db.models import Q
from django.views.generic import View
from django.shortcuts import render_to_response
from yoolotto.rest import exceptions
from yoolotto.rest.decorators import rest, Authenticate
from django.core.mail import send_mail
from yoolotto.coupon.forms import CouponGeographyLookupForm
from yoolotto.coupon.geo.manager import GeoManager
from yoolotto.coupon.models import CouponIssue, Coupon
from yoolotto.coupon.models import CouponVote as CouponVoteModel
from yoolotto.coupon.models import CouponVendor as CouponVendorModel
from yoolotto.coupon.models import CouponGeography as CouponGeographyModel
from yoolotto.coupon.models import Coupon as CouponModel
from yoolotto.user.models import Device
from yoolotto.coupon.sendemail import common_send_email
from yoolotto.lottery.models import LotteryTicket
from yoolotto.settings_local import COUPON_MAIL_CC, COUPON_REDEEM_DURATION



class CouponWallet(View):
    @rest
    @Authenticate()
    def get(self, request, filter="active"):
        
        import datetime
        user = request.yoo["user"]
        now = datetime.datetime.now()
        cv_id = request.GET.get('cv_id', None)
        vendor = request.GET.get("vendor", None)
        
        if filter == "active":
            issued = CouponIssue.objects.filter(user=user, redeemed__isnull=True, 
                coupon__redeemable=True, coupon__valid_to__gte=now)

            if len(issued) < 5:
                for x in xrange(5):
                    CouponIssue.assign(ticket=None, user=user)
            
                issued = CouponIssue.objects.filter(user=user, redeemed__isnull=True, 
                    coupon__redeemable=True, coupon__valid_to__gte=now)
                                        
            if vendor:
                try:
                    issued = issued.filter(coupon__vendor_id=int(vendor))
                except ValueError:
                    pass
                
            issued = list(issued)
            redeemed = CouponIssue.objects.filter(user=user, redeemed__isnull=False)

            if vendor:
                try:
                    issued = redeemed.filter(coupon__vendor_id=int(vendor))
                except ValueError:
                    pass
            if not cv_id:
                return map(lambda x: x.representation(), issued + list(redeemed))
            else:
                couponvendor = CouponVendorModel.objects.get(id=cv_id)
                try:
                    coupon = CouponModel.objects.filter(vendor=couponvendor,
                                valid_from__lte=now,
                                valid_to__gte=now,
                                redeem_limit__gte=1,
                                )
                except Exception as e:
                    print e
                    raise exceptions.WebServiceException("No coupons available for this Vendor")
                if coupon:
                    return map(lambda x: x.representation(), coupon)
                else:
                    raise exceptions.WebServiceException("No coupons available for this Vendor")
                
                
        elif filter == "redeemed":
            redeemed = CouponIssue.objects.filter(user=user, redeemed__isnull=False)
            return map(lambda x: x.representation(), redeemed)
        elif filter == "expired":
            expired = CouponIssue.objects.filter(
                Q(Q(user=user)&Q(redeemed__isnull=True))
                &Q(Q(coupon__valid_to__lte=now)|Q(coupon__redeemable=False)))
            
            return map(lambda x: x.representation(), expired)
        else:
            raise exceptions.WebServiceException("Invalid Coupon Filter")
        
class CouponRedeem(View):
    
    def get(self, request, _id):
        
        # Short-Circuit for Authentication Errors caused by invalid Device IDs
        if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            return render_to_response("redeem_error.html")
        
        issue = CouponIssue.objects.get(pk=_id)
        
        try:
            device = Device.objects.get(device_id=request.META["HTTP_YOO_DEVICE_ID"])                
        except Device.DoesNotExist:
            return render_to_response("redeem_error.html")
        except KeyError:
            return render_to_response("redeem_error.html")
        
        if device.user != issue.user:
            return render_to_response("redeem_error.html")
        
        import json
        
        # Template Parameters
        params = {"issue": issue}
        
        percent = int(math.ceil((float(issue.coupon.issued_redeemed) / issue.coupon.redeem_limit) * 100))
        percent = percent if percent > 15 else 15
        remainder = 100 - percent
        
        params["percent"] = percent
        params["remainder"] = remainder
        params["from"] = issue.coupon.valid_from.strftime("%m/%y")
        params["to"] = issue.coupon.valid_to.strftime("%m/%y")
        
        if not issue.redeemed:
            issue.redeemed = datetime.datetime.now()
            issue.save()
        
        params["time"] = issue.representation()["remaining"]
                
        return render_to_response("redeem.html", params)
        
    @rest
    @Authenticate()
    def post(self, request, _id):
        user = request.yoo["user"]
        issue = CouponIssue.objects.get(pk=_id)
        
        if issue.user != user:
            raise exceptions.WebServiceAuthorizationFailed()
        
        if not issue.redeemed:
            issue.redeemed = datetime.datetime.now()
            issue.save()
        
        return issue.representation()


class CouponVote(View):
    @rest
    @Authenticate()
    def post(self, request, _id, _cv_id, _coupon_id):
        # giving some variables
        import datetime
        return_data = {}
        now = datetime.datetime.now()
        # basic authentication
        if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            return render_to_response("redeem_error.html")
        try:
            device = Device.objects.get(device_id=request.META["HTTP_YOO_DEVICE_ID"])
        except Device.DoesNotExist:
            return render_to_response("redeem_error.html")
        except KeyError:
            return render_to_response("redeem_error.html")
        try:
            coupon=CouponModel.objects.get(id=_coupon_id, redeem_limit__gte=1)
        except Exception as e:
            raise exceptions.WebServiceException("All coupons are redeemed.Better Luck next time")
        data = json.loads(request.body)
        user = request.yoo["user"]
        try:
            # First check if there is any coupon issued to this user with the latest date
            # this simply means that the user is old
            issue = CouponIssue.objects.filter(user=request.yoo["user"]).latest('added_at')
            issue_date = issue.added_at
            print issue_date
            #check for 24 hours
            from datetime import datetime, timedelta
            # Validity of a user to redeem another coupon is 24 hours
            # so add 24 hours to it.
            try:
                
                date_valid = issue.redeemed + timedelta(minutes=COUPON_REDEEM_DURATION) 
                coupon_redeemed = 1
            except Exception as e:
                date_valid =  issue_date + timedelta(minutes=COUPON_REDEEM_DURATION)
                coupon_redeemed = 0
            if coupon_redeemed == 1:
                raise exceptions.WebServiceException("You have redeemed the coupon for thekhhghjfg day.Wait for next 24 hours")
            else:
                try:
                    # Get the Vendor from Request
                    #couponvendor_obj = CouponVendorModel.objects.get(pk=_cv_id)
                    coupon_obj = CouponModel.objects.get(id=_coupon_id,redeem_limit__gte=1)
                    # Increase the Issue Total and Decrease the Redeem Limit
                    if coupon_obj.redeem_limit >= 1:
                        coupon_obj.issued_total = int(coupon_obj.issued_total) + 1
                        coupon_obj.redeem_limit = int(coupon_obj.redeem_limit) - 1
                        #coupon_obj.issued_redeemed = int(coupon_obj.issued_redeemed) + 1
                        coupon_obj.save()
                    # raise exception
                    elif int(coupon_obj.redeem_limit) < int(coupon_obj.issued_total):
                        raise exceptions.WebServiceException("No coupons available")
                    # raise exception
                    else:
                        raise exceptions.WebServiceException("No coupons available")
                    
                    # Now finally issue this coupon to the current user
                    issue, created = CouponIssue.objects.get_or_create(user=request.yoo["user"],
                                                                        redeemed=now,
                                                                        coupon_id=_coupon_id,
                                                                        )
                    # Now assign the current ticket to this coupon issue obj
                    if not _id == "1":
                        if not issue.ticket_id:
                            ticket_obj = LotteryTicket.objects.get(id=_id)
                            issue.ticket = ticket_obj
                            issue.save()
                    subject = "YooCoupons"
                    recipients = [data['email']]+COUPON_MAIL_CC
                    context = {}
                    text_template_path = "coupon_email.txt"
                    html_template_path = "common_vendor_email.html"
                    context_data = {'coupon_obj': coupon_obj}
                    try:
                        common_send_email(subject, text_template_path, html_template_path,context_data, recipients)
                        return_data['email'] = 1
                    except Exception as e:
                        return_data['email'] = 0
                    return return_data
                except Exception as e:
                    # this means that the user is coming for the first time
                    # Issue this coupon to current user and assign this ticket id.
                    coupon=CouponModel.objects.get(id=_coupon_id)
                    if coupon.redeem_limit >= 1:    
                        issue, created = CouponIssue.objects.get_or_create(user=request.yoo["user"],
                                                                            redeemed=now,
                                                                            coupon=coupon,
                                                                            )
                        # Now assign the current ticket to this coupon issue obj
                        if not _id == "1":
                            if not issue.ticket_id:
                                ticket_obj = LotteryTicket.objects.get(id=_id)
                                issue.ticket = ticket_obj
                                issue.save()
                        issue_date = issue.redeemed
                        coupon_obj = CouponModel.objects.get(id=_coupon_id)
                        # Increase the Issue Total and Decrease the Redeem Limit
                        if coupon_obj.redeem_limit >= 1:
                            coupon_obj.issued_total = int(coupon_obj.issued_total) + 1
                            coupon_obj.redeem_limit = int(coupon_obj.redeem_limit) - 1
                            #coupon_obj.issued_redeemed = int(coupon_obj.issued_redeemed) + 1
                            coupon_obj.save()
                        # now finally send the email.
                        try:
                            #common_send_email(subject, text_template_path, html_template_path,context_data, recipients)
                            return_data['email'] = 1
                        except Exception as e:
                            return_data['email'] = 0
                    else:
                        raise exceptions.WebServiceException("All coupons are redeemed.Better Luck next time")
    
        except Exception as e:
            data = json.loads(request.body)
            user = request.yoo["user"]
            return_data = {}
            print e
            print _coupon_id
            print _id
            try:
                issue = CouponIssue.objects.filter(user=request.yoo["user"]).latest('added_at')
                issue_date = issue.added_at
                print issue_date
            except Exception as e:
                pass
            #try:
                
            #    date_valid = issue.redeemed + timedelta(hours=24) 
            #    coupon_redeemed = 1
            #except Exception as e:
            #    date_valid =  issue_date + timedelta(hours=24)
            #    coupon_redeemed = 0
            #if coupon_redeemed == 1:
            #    raise exceptions.WebServiceException("You have redeemed the coupon for thekhhghjfg day.Wait for next 24 hours")
            coupon=CouponModel.objects.get(id=_coupon_id)
            if coupon.redeem_limit >= 1:    
                issue, created = CouponIssue.objects.get_or_create(user=request.yoo["user"],
                                                                    redeemed=now,
                                                                    coupon=coupon,
                                                                    )
                print  issue
                # Now assign the current ticket to this coupon issue obj
                if not _id == "1":
                    if not issue.ticket_id:
                        ticket_obj = LotteryTicket.objects.get(id=_id)
                        issue.ticket = ticket_obj
                        issue.save()
                issue_date = issue.redeemed
                coupon_obj = CouponModel.objects.get(id=_coupon_id)
                # Increase the Issue Total and Decrease the Redeem Limit
                if coupon_obj.redeem_limit >= 1:
                    coupon_obj.issued_total = int(coupon_obj.issued_total) + 1
                    coupon_obj.redeem_limit = int(coupon_obj.redeem_limit) - 1
                    #coupon_obj.issued_redeemed = int(coupon_obj.issued_redeemed) + 1
                    coupon_obj.save()
                
                subject = "YooCoupons"
                recipients = [data['email']]+COUPON_MAIL_CC
                #recipients = data['email']
                context = {}
                text_template_path = "coupon_email.txt"
                html_template_path = "coupon_email.html"
                context_data = {'coupon_obj': coupon_obj}
                # now finally send the email.
                try:
                    common_send_email(subject, text_template_path, html_template_path,context_data, recipients)
                    return_data['email'] = 1
                except Exception as e:
                    return_data['email'] = 0
                #common_send_email(subject, text_template_path, html_template_path,context_data, recipients)
                #return_data['email'] = 1
                return return_data
            else:
                raise exceptions.WebServiceException("All coupons are redeemed.Better Luck next time")

    def put(self, *args, **kwargs):
        return self.post(*args, **kwargs)
    


class RedeemCoupon(View):
    @rest
    @Authenticate()
    def post(self, request, _id, _cv_id, _coupon_id):
        # giving some variables
	
        import datetime
        return_data = {}
        now = datetime.datetime.now()
        # basic authentication
        if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            return render_to_response("redeem_error.html")
        try:
        	device = Device.objects.get(device_id=request.META["HTTP_YOO_DEVICE_ID"])
        except Device.DoesNotExist:
            return render_to_response("redeem_error.html")
        except KeyError:
            return render_to_response("redeem_error.html")
        data = json.loads(request.body)
        user = request.yoo["user"]
        # checking it here so that vendor is available throughout.
        try:
            vendor = CouponVendorModel.objects.get(id=_cv_id)
        except Exception as e:
            raise exceptions.WebServiceException("Vendor DoesNotExist")
        # checking it here so that coupon is available throughout.
        try:
           coupon=CouponModel.objects.get(id=_coupon_id, redeem_limit__gte=1)
        except Exception as e:
            raise exceptions.WebServiceException("All coupons are redeemed.Better Luck next time")

        # checking it here so that ticket is available throughout.
	try:
            ticket = LotteryTicket.objects.get(id=_id)
            
        except Exception as e:
            pass
	
        
        # First check if there is any coupon issued to this user with the latest date
        # this simply means that the user is old
	
        from yoolotto.user.models import YooLottoUser
        try:
            issue = CouponIssue.objects.filter(user=user).latest('added_at')
        except Exception as e:
            try:
               user_obj, created = YooLottoUser.objects.get_or_create(name=user,email=data['email'])
            except:
               pass
            
            issue = CouponIssue.objects.create(user=user)
	
        if issue.redeemed:
            from datetime import datetime, timedelta
            
            issue_date = issue.redeemed
            valid_date = issue_date + timedelta(minutes=COUPON_REDEEM_DURATION)
            if now < valid_date:
                return_data['email'] = 0
                return return_data
            
        else:
            if coupon.redeem_limit >= 1:
                issue.redeemed = now
                issue.coupon = coupon
                issue.save()
                           
                # Now assign the current ticket to this coupon issue obj
                if not _id == "1":
                    if not issue.ticket_id:
                        
                        issue.ticket = ticket
                        issue.save()
                issue_date = issue.redeemed
                
                # Increase the Issue Total and Decrease the Redeem Limit
                coupon.issued_total = int(coupon.issued_total) + 1
                coupon.redeem_limit = int(coupon.redeem_limit) - 1
                coupon.save()
                
                subject = "YooCoupons"
                recipients = [data['email']]+COUPON_MAIL_CC
                #recipients = data['email']
                context = {}
                text_template_path = "coupon_email.txt"
                html_template_path = "coupon_email.html"
                context_data = {'coupon_obj': coupon}
                # now finally send the email.
		
                try:
                   common_send_email(subject, text_template_path, html_template_path,context_data, recipients)
	           return_data['email'] = 1
                except Exception as e:
                    
                    raise exceptions.WebServiceException("All coupons are redeemed.Better Luck next time")
                
                return return_data
            else:
                raise exceptions.WebServiceException("All coupons are redeemed.Better Luck next time")
        

        


class CouponGeography(View):
    @rest
    @Authenticate()
    def get(self, request, _id):
        user = request.yoo["user"]
        
        _id = int(_id)
        
        form = CouponGeographyLookupForm(request.GET)
        if not form.is_valid():
            raise exceptions.WebServiceFormValidationFailed(form)
        
        if _id > 0:
            issue = CouponIssue.objects.get(pk=_id)
            
            if issue.user != user:
                raise exceptions.WebServiceAuthorizationFailed()
        
            if issue.coupon.geography:
                try:
                    handler = GeoManager.get(issue.coupon.geography.format)
                except:
                    raise exceptions.WebServiceException("Coupon has no Geography")
            else:
                try:
                    handler = GeoManager.get("7Eleven")
                except:
                    raise exceptions.WebServiceException("Coupon has no Geography")
                
            return handler.retrieve(form.cleaned_data["latitude"], form.cleaned_data["longitude"])
            
        else:
            handlers = []
            for handler in CouponGeographyModel.objects.filter(active=True):
                try:
                    handlers.append(GeoManager.get(handler.format))
                except:
                    pass
                
            locations = []
            map(lambda x: locations.extend(x.retrieve(
                form.cleaned_data["latitude"], form.cleaned_data["longitude"])), handlers)
            
            locations.sort(key=lambda x: x["distance"])
                
            return locations
        
        
    
class CouponVendor(View):
    @rest
    @Authenticate()
    def get(self, request, _id=None):
        user = request.yoo["user"]
        
        if not _id:
            import datetime
            now=datetime.datetime.now()
            #coupon = CouponModel.objects.filter(user=user,redeemable=True).latest('added_at')
            #print coupon
            coupon_vendors = CouponVendorModel.objects.all()
            if coupon_vendors:
            	return map(lambda x: x.representation(user=user),coupon_vendors)
        
        return []


class CouponSendEmail(View):
    @rest
    @Authenticate()
    def post(self, request, _id):
        user = request.yoo["user"]
        try:
            data = json.loads(request.body)
        except:
            data = {}

        couponvendor_obj = CouponVendor.objects.get(pk=_id)
        coupon_obj = Coupon.objects.get(vendor=couponvendor_obj)
        subject = "YooCoupons"
        recipients = data['email']
        context = {}
        text_template_path = "coupon_email_content.txt"
        html_template_path = "common_vendor_email.html"
        context_data = {'coupon_obj': coupon_obj}
        common_send_email(subject, text_template_path, html_template_path,
                                        context_data, recipients)
        
    
    def put(self, *args, **kwargs):
        return self.post(*args, **kwargs)
    
