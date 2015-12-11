from django.core.management.base import BaseCommand, CommandError

from yoolotto.lottery.feed.data import LotteryTicketDataProvider
from yoolotto.lottery.feed.importer import LotteryTicketImporter


class Command(BaseCommand):
    help = 'Manual trigger for performing feed updates  '

    def handle(self, *args, **options):
        importer = LotteryTicketImporter(provider=LotteryTicketDataProvider.remote)
#        importer = LotteryTicketImporter(provider=LotteryTicketDataProvider.sample)
        importer.run()
    