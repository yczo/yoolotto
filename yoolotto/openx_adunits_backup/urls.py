from django.conf.urls import patterns,url
from yoolotto.openx_adunits.views import adUnit
urlpatterns = patterns('',
      url('^adunits',adUnit.as_view()),
)

