from django.conf.urls import patterns, include, url

from yoolotto.user.views import *

urlpatterns = patterns('',
    url('^$', User.as_view()),
    url('^device$', Device.as_view()),
    url('^preference', Preference.as_view()),
    url('^referrals', ReferralStats.as_view()),
    url('^register',RegisterUser.as_view()),
    url('^login',LoginUser.as_view()),
    url('^logout',LogoutUser.as_view()),
    url('^forgot_password',ForgotPassword.as_view()),
    url('^verify_password_code',VerifyPasswordCode.as_view()),
    url('^reset_password',ResetPassword.as_view()),
    url('^services', Services.as_view()),
    url('^yoo_login', YooLoginaa.as_view()),
    url('^home', Home.as_view()),
    url('^yoo_logout', YooLogout.as_view()),



)
