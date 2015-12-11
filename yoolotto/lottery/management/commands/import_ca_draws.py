from django.core.management.base import BaseCommand, CommandError

from yoolotto.lottery.feed.data import LotteryTicketDataProvider
from yoolotto.lottery.feed.importer import LotteryTicketImporter
from yoolotto.lottery.feed.import_ca_games import CALotteryTicketImporter

class Command(BaseCommand):
    help = 'Manual trigger for performing feed updates  '

    def handle(self, *args, **options):
        importer_ca = CALotteryTicketImporter(provider=LotteryTicketDataProvider.remote)
        importer_ca.run()       
    