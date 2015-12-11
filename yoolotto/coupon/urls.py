from django.conf.urls import patterns, include, url

from yoolotto.coupon.views import *

urlpatterns = patterns('',
    url('^wallet/(?P<filter>.+)$', CouponWallet.as_view()),    
    url('^(?P<_id>\d+)/redeem$', CouponRedeem.as_view()), 
    url('^(?P<_id>\d+)/geography', CouponGeography.as_view()),
    url('^(?P<_id>\d+)/vote/(?P<_cv_id>\d+)/(?P<_coupon_id>\d+)', RedeemCoupon.as_view()), 
    url('^vendor/(?P<_id>\d+)?$', CouponVendor.as_view()), 
)