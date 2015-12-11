from django.core.management.base import BaseCommand, CommandError

from yoolotto.legacy.migration.draws import MigrateDraws

class Command(BaseCommand):
    def handle(self, *args, **options):
        MigrateDraws().run()