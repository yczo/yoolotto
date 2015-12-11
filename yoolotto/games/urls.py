from django.conf.urls import patterns, include, url

from yoolotto.games.views import *

urlpatterns = patterns('',
    url('^$', GamesStub.as_view())
)