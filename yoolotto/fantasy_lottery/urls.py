from django.conf.urls import patterns, include, url
from yoolotto.lottery.feed.manual_push import ManualNotification, FindTickets,\
    FindUser, FindLastUser
from yoolotto.lottery.views import *
from yoolotto.fantasy_lottery.views import *


urlpatterns = patterns('',
    url('enumerated_data$', enumeration_data.as_view()),
    url('quick_pick$', QuickPick.as_view()),
    url('submit_no$',SubmitNumbers.as_view()),
    url('check_ticket$',FantasyTicketCheck.as_view())

    )