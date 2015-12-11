from django.conf.urls import patterns, include, url
from yoolotto.promo.views import PromoLanding
from yoolotto.communication.views import Notification
from yoolotto.util.views import *
from yoolotto.openx_adunits.views import in_app_fuel, YooGames
from yoolotto.second_chance.views import *
from yoolotto.openx_adunits.views import SCACoins,SCATest,EmailVerifyyy
# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    url('^ad', include('yoolotto.ad.urls')),
    url('^yoogames','yoolotto.second_chance.views.yoo_games_webpage'),
    url('^verify_email', EmailVerifyyy.as_view()),
    url('^coupon/', include('yoolotto.coupon.urls')),
    url('^second_chance/', include('yoolotto.second_chance.urls')),
    url('^lottery/', include('yoolotto.lottery.urls')),
    url('^user/', include('yoolotto.user.urls')),
    #url('^fantasy_lottery/', include('yoolotto.fantasy_lottery.urls')),
    url('^games', include('yoolotto.games.urls')),
    url('^coingames', include('yoolotto.games.urls')),
    url('^promo$', PromoLanding.as_view()),
    url('^promo/(?P<mode>.*)', PromoLanding.as_view()),
    
    url('^private/communication/notification', Notification.as_view()),
    url('^_util/version', BuildVersion.as_view()),
    url(r'^admin/', include(admin.site.urls)),
    url(r'^analytics/', 'yoolotto.analytics.views.generate_report', name="analytics"),
    url(r'^notification/', 'yoolotto.analytics.views.notifications', name="notifications"),
    url(r'^winnings/', 'yoolotto.analytics.views.winnings', name="winnings"),
    url('^openx/', include('yoolotto.openx_adunits.urls')),
    url(r'^bulk_notification/', 'yoolotto.yoolotto_debug.views.send_bulk_notification'),
    url('^other/in_app_fuel',in_app_fuel.as_view()),
    url('^sca_coins',SCACoins.as_view()),
    url('^sca_test',SCATest.as_view()),
    url(r'^dte_images/', 'yoolotto.yoolotto_debug.views.data_entry_images'),
    url(r'^yoo_games', YooGames.as_view()),

    
    
    
)

