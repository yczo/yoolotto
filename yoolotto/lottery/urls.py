from django.conf.urls import patterns, include, url
from yoolotto.lottery.feed.manual_push import ManualNotification, FindTickets,\
    FindUser, FindLastUser
from yoolotto.lottery.views import *
from yoolotto.lottery.ticket.views import *


urlpatterns = patterns('',
    url('^game/(?P<id>\d+)$', Game.as_view()),
    url('^game/draw/(?P<_id>\d+)/check$', GameCheck.as_view()),
    url('^yoo_game/draw/(?P<_id>\d+)/ticketID/(?P<_id1>\d+)/check$', YooGameCheck.as_view()),
    url('^game$', Game.as_view()),
    url('^play$', Play_Game.as_view()),
    url('^submit$', SubmitAgain.as_view()),
    url('^edit$', EditData.as_view()),
    url('^ticket/(?P<_id>\d+)/check', TicketCheck.as_view()),
    url('^ticket/(?P<_id>\d+)/share', TicketShare.as_view()),
    url('^ticket/(?P<_id>\d+)/delete', TicketDelete.as_view()),
    url('^ticket', Ticket.as_view()),
    url('^manual_notification/last_user',FindLastUser.as_view()),
    url('^manual_notification/find_tickets',FindTickets.as_view()),
    url('^manual_notification/find_user',FindUser.as_view()),
    url('^manual_notification',ManualNotification.as_view()),
    url('^winner',Winner.as_view()),
    url('enumerated_data$', enumeration_data.as_view()),
    url('quick_pick$', QuickPick.as_view()),
    url('submit_no$',SubmitNumbers.as_view()),
    url('help_info$',FantasyHelpInfo.as_view()),
    url('game_info$',FantasyGameInfo.as_view()),
    url('user_profile$',UserProfile.as_view()),
    url('yoocoins_info',YoocoinsInfo.as_view()),
    url('user_testimonials',Testimonials.as_view()),
    url('rules_and_info', Rules.as_view()),
    url('^yoo_ticket', YooTicket.as_view()),
    url('^fantasy_winner', Fantasy_Winner.as_view()),
    
    
#    url('^login$', UserLogin.as_view()),
)
