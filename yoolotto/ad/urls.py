from django.conf.urls import patterns, include, url

from yoolotto.ad.views import *

urlpatterns = patterns('',
    url('^$', AdvertisementRetriever.as_view())
)