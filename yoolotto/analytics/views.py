from yoolotto.analytics.forms import AnalyticForm
from django.shortcuts import render
from django.db import connection
from django.http import HttpResponse
import csv
from django.shortcuts import redirect


def notifications(request):
    cursor = connection.cursor()
    query = """
    SELECT lottery_draw.date, lottery_draw.jackpot, lottery_game_component.name,
    COUNT(1) as notified FROM lottery_ticket INNER JOIN lottery_draw ON
    lottery_draw.id = lottery_ticket.draw_id INNER JOIN lottery_game_component ON
    lottery_game_component.id = lottery_draw.component_id WHERE notified = 1 GROUP BY
    draw_id ORDER BY notified DESC;
    """
    

    response = HttpResponse(mimetype='text/csv')
    response['Content-Disposition'] = 'attachment;filename=notifications.csv'
    writer = csv.writer(response)
    writer.writerow(['Date','Jackpot','Game','Notifications'])
    result_count = cursor.execute(query)
    result = cursor.fetchall()
    if request.method == "POST":
        for notifications in result:
            data = [notifications[0], notifications[1],notifications[2],notifications[3]]
            writer.writerow(data)
        return response

    context = {'result':result, 'result_count':result_count}
    template = "analytics/notifications.html"
    return render(request, template, context)


def winnings(request):
    query = """
    SELECT lottery_draw.date, lottery_draw.jackpot, lottery_game_component.name,
    lottery_ticket.winnings FROM lottery_ticket INNER JOIN lottery_draw ON
    lottery_draw.id = lottery_ticket.draw_id INNER JOIN lottery_game_component
    ON lottery_game_component.id = lottery_draw.component_id WHERE winnings
    > 0 AND user_id NOT IN (11, 25, 1131, 2667, 10538, 1568) ORDER BY winnings DESC;
    """
    cursor = connection.cursor()
    response = HttpResponse(mimetype='text/csv')
    response['Content-Disposition'] = 'attachment;filename=winnings.csv'
    writer = csv.writer(response)
    writer.writerow(['Date','Jackpot','Game','Winnings'])
    result_count = cursor.execute(query)
    result = cursor.fetchall()
    if request.method == "POST":
        for winnings in result:
            data = [winnings[0], winnings[1], winnings[2], winnings[3]]
            writer.writerow(data)
        return response
    
    template = "analytics/winnings.html"
    context = {'result_count':result_count, 'result':result}
    return render(request, template, context)

def powerball_stats():
    query = """
    SELECT "Total Players" as data, COUNT(DISTINCT user_id) as count
    FROM lottery_ticket INNER JOIN lottery_draw ON lottery_draw.id
    = lottery_ticket.draw_id WHERE lottery_draw.component_id
    = 1 UNION SELECT "Total Plays" as data, COUNT(1) as count FROM
    lottery_ticket INNER JOIN lottery_draw ON lottery_draw.id
    = lottery_ticket.draw_id WHERE lottery_draw.component_id = 1
    UNION SELECT "Total Winning Plays" as data, COUNT(1) as count
    FROM lottery_ticket INNER JOIN lottery_draw ON lottery_draw.id
    = lottery_ticket.draw_id WHERE lottery_draw.component_id = 1
    AND lottery_ticket.winnings > 0 UNION SELECT "Total Lines" as
    data, COUNT(1) as count FROM lottery_ticket INNER JOIN lottery_draw
    ON lottery_draw.id = lottery_ticket.draw_id LEFT OUTER JOIN
    lottery_ticket_play ON lottery_ticket_play.ticket_id
    = lottery_ticket.id WHERE lottery_draw.component_id = 1;
    """
    cursor = connection.cursor()
    
    result_count = cursor.execute(query)
    result = cursor.fetchall()
    return result

def megamillion_stats():
    query = """
    SELECT "Total Players" as data, COUNT(DISTINCT user_id) as count FROM
    lottery_ticket INNER JOIN lottery_draw ON lottery_draw.id
    = lottery_ticket.draw_id WHERE lottery_draw.component_id = 2
    UNION SELECT "Total Plays" as data, COUNT(1) as count FROM lottery_ticket
    INNER JOIN lottery_draw ON lottery_draw.id = lottery_ticket.draw_id WHERE
    lottery_draw.component_id = 2 UNION SELECT "Total Winning Plays" as data,
    COUNT(1) as count FROM lottery_ticket INNER JOIN lottery_draw ON
    lottery_draw.id = lottery_ticket.draw_id WHERE lottery_draw.component_id = 2
    AND lottery_ticket.winnings > 0 UNION SELECT "Total Lines" as data, COUNT(1)
    as count FROM lottery_ticket INNER JOIN lottery_draw ON
    lottery_draw.id = lottery_ticket.draw_id LEFT OUTER JOIN lottery_ticket_play
    ON lottery_ticket_play.ticket_id = lottery_ticket.id WHERE
    lottery_draw.component_id = 2;
    """
    cursor = connection.cursor()
    result_count = cursor.execute(query)
    result = cursor.fetchall()
    return result

def cashfive_stats():
    query = """
    SELECT "Total Players" as data, COUNT(DISTINCT user_id) as count FROM
    lottery_ticket INNER JOIN lottery_draw ON lottery_draw.id = lottery_ticket.draw_id
    WHERE lottery_draw.component_id = 10 UNION SELECT "Total Plays" as data,
    COUNT(1) as count FROM lottery_ticket INNER JOIN lottery_draw ON lottery_draw.id
    = lottery_ticket.draw_id WHERE lottery_draw.component_id = 10 UNION SELECT
    "Total Winning Plays" as data, COUNT(1) as count FROM lottery_ticket INNER
    JOIN lottery_draw ON lottery_draw.id = lottery_ticket.draw_id WHERE
    lottery_draw.component_id = 10 AND lottery_ticket.winnings > 0 UNION SELECT
    "Total Lines" as data, COUNT(1) as count FROM lottery_ticket INNER JOIN lottery_draw
    ON lottery_draw.id = lottery_ticket.draw_id LEFT OUTER JOIN lottery_ticket_play ON
    lottery_ticket_play.ticket_id = lottery_ticket.id WHERE lottery_draw.component_id = 10;
    """
    cursor = connection.cursor()
    result_count = cursor.execute(query)
    result = cursor.fetchall()
    return result  

def twostep_stats():

    query = """
    SELECT "Total Players" as data, COUNT(DISTINCT user_id)
    as count FROM lottery_ticket INNER JOIN lottery_draw ON
    lottery_draw.id = lottery_ticket.draw_id WHERE lottery_draw.component_id
    = 5 UNION SELECT "Total Plays" as data, COUNT(1) as count FROM lottery_ticket
    INNER JOIN lottery_draw ON lottery_draw.id = lottery_ticket.draw_id WHERE
    lottery_draw.component_id = 5 UNION SELECT "Total Winning Plays" as data,
    COUNT(1) as count FROM lottery_ticket INNER JOIN lottery_draw ON
    lottery_draw.id = lottery_ticket.draw_id WHERE lottery_draw.component_id
    = 5 AND lottery_ticket.winnings > 0 UNION SELECT "Total Lines" as data,
    COUNT(1) as count FROM lottery_ticket INNER JOIN lottery_draw ON
    lottery_draw.id = lottery_ticket.draw_id LEFT OUTER JOIN lottery_ticket_play
    ON lottery_ticket_play.ticket_id = lottery_ticket.id WHERE lottery_draw.component_id = 5;
    """
    cursor = connection.cursor()
    result_count = cursor.execute(query)
    result = cursor.fetchall()
    return result


def generate_report(request):
    """
    Generate the Analytics
    """
    stats = None
    if request.method == "POST":
        form_data = AnalyticForm(request.POST)
        if form_data.is_valid():
            cd = form_data.cleaned_data
            if cd['game'] == "Powerball":
                stats = powerball_stats()
            elif cd['game'] == "MEGA Millions":
                stats = megamillion_stats()
            elif cd['game'] == "Pick Three":
                stats = pickthree_stats()
            elif cd['game'] == "All or Nothing":
                stats = allornothing_stats()
            elif cd['game'] == "Cash Five":
                stats = cashfive_stats()
            elif cd['game'] == "Daily Four":
                stats = dailyfour_stats
            elif cd['game'] == "Two Step":
                stats = twostep_stats()
            elif cd['game'] == "Lotto Texas":
                stats = lotto_stats()
            if 'export' in request.POST:
                response = HttpResponse(mimetype='text/csv')
                response['Content-Disposition'] = 'attachment;filename=%s.csv' % cd['game']
                writer = csv.writer(response)
                writer.writerow(['Total Players','Total Plays','Total Winning Plays','Total Lines'])
                heading_list = []
                value_list = []
                data_list = []
                for analytic in stats:
                    data_list.append(list(analytic)[1])
                writer.writerow(data_list)
                return response
            template = "analytics/generate_report.html"
            context = {'active':'active', 'form_data':form_data, 'stats':stats}
            return render(request, template, context)
    else:
        form_data = AnalyticForm()
    template = "analytics/generate_report.html"
    context = {'active':'active', 'form_data':form_data, 'stats':stats}
    return render(request, template, context)
