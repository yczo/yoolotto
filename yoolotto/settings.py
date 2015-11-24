import os

"""
Absolute Installation Paths

"""

SITE_ROOT = os.path.dirname(os.path.realpath(__file__))
SITE_CONTAINER = os.path.dirname(SITE_ROOT)

PROJECT_STATIC_DIR=os.path.join(SITE_CONTAINER, "static")# project static root directory
PROJECT_TEMPLATE_DIR=os.path.join(SITE_ROOT, "templates")# project templates root directory

LOG_BASE_PATH = os.path.join(SITE_CONTAINER, "log")
IMPORT_LOG_PATH = os.path.join(SITE_CONTAINER, "import")


DEBUG = True
TEMPLATE_DEBUG = DEBUG

"""
Email Sending
"""
ERROR_NOTIFICATION = not DEBUG
ERROR_EMAIL = ["irfan@spa-systems.com",]

POSTMARK_API_KEY = "bab5362e-a9d2-4dfe-974c-8fc95bf93b79"
POSTMARK_SENDER = "YooLotto <alert@yoolotto.com>"

ADMINS = (
    # ('Your Name', 'your_email@example.com'),
)

MANAGERS = ADMINS

DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.mysql', # Add 'postgresql_psycopg2', 'mysql', 'sqlite3' or 'oracle'.
        'NAME': 'main',                      # Or path to database file if using sqlite3.
        # The following settings are not used with sqlite3:
        'USER': 'apimaster',
        'PASSWORD': 'nLdyj8WV8xihEL',
        'HOST': 'api.cqvuojilubcb.us-east-1.rds.amazonaws.com',                      # Empty for localhost through domain sockets or '127.0.0.1' for localhost through TCP.
        'PORT': '',                      # Set to empty string for default.
    }
}

# Hosts/domain names that are valid for this site; required if DEBUG is False
# See https://docs.djangoproject.com/en/1.5/ref/settings/#allowed-hosts
ALLOWED_HOSTS = [".yoolotto.com"]

# Local time zone for this installation. Choices can be found here:
# http://en.wikipedia.org/wiki/List_of_tz_zones_by_name
# although not all choices may be available on all operating systems.
# In a Windows environment this must be set to your system time zone.
TIME_ZONE = 'UTC'

# Language code for this installation. All choices can be found here:
# http://www.i18nguy.com/unicode/language-identifiers.html
LANGUAGE_CODE = 'en-us'

SITE_ID = 1

# If you set this to False, Django will make some optimizations so as not
# to load the internationalization machinery.
USE_I18N = True

# If you set this to False, Django will not format dates, numbers and
# calendars according to the current locale.
USE_L10N = True

# If you set this to False, Django will not use timezone-aware datetimes.
USE_TZ = False

# Absolute filesystem path to the directory that will hold user-uploaded files.
# Example: "/var/www/example.com/media/"
MEDIA_ROOT = ''

# URL that handles the media served from MEDIA_ROOT. Make sure to use a
# trailing slash.
# Examples: "http://example.com/media/", "http://media.example.com/"
MEDIA_URL = ''

# Absolute path to the directory static files should be collected to.
# Don't put anything in this directory yourself; store your static files
# in apps' "static/" subdirectories and in STATICFILES_DIRS.
# Example: "/var/www/example.com/static/"
STATIC_ROOT = ''

# URL prefix for static files.
# Example: "http://example.com/static/", "http://static.example.com/"
STATIC_URL = '/static/'

# Additional locations of static files
STATICFILES_DIRS = (
                    PROJECT_STATIC_DIR,
    # Put strings here, like "/home/html/static" or "C:/www/django/static".
    # Always use forward slashes, even on Windows.
    # Don't forget to use absolute paths, not relative paths.
)

# List of finder classes that know how to find static files in
# various locations.
STATICFILES_FINDERS = (
    'django.contrib.staticfiles.finders.FileSystemFinder',
    'django.contrib.staticfiles.finders.AppDirectoriesFinder',
#    'django.contrib.staticfiles.finders.DefaultStorageFinder',
)

# Make this unique, and don't share it with anybody.
SECRET_KEY = '06#)fvav+k7ald)zxhg%!6vnjtg1#^7cj$hf6rmy%akpfwx$a5'

# List of callables that know how to import templates from various sources.
TEMPLATE_LOADERS = (
    'django.template.loaders.filesystem.Loader',
    'django.template.loaders.app_directories.Loader',
#     'django.template.loaders.eggs.Loader',
)

MIDDLEWARE_CLASSES = (
    'django.middleware.common.CommonMiddleware',
    'django.contrib.sessions.middleware.SessionMiddleware',
#    'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    
    #'debug_toolbar.middleware.DebugToolbarMiddleware',
    # Uncomment the next line for simple clickjacking protection:
    # 'django.middleware.clickjacking.XFrameOptionsMiddleware',
)

ROOT_URLCONF = 'yoolotto.urls'

# Python dotted path to the WSGI application used by Django's runserver.
WSGI_APPLICATION = 'yoolotto.wsgi.application'

TEMPLATE_DIRS = (
                 PROJECT_TEMPLATE_DIR,
                 PROJECT_STATIC_DIR+"/coupon/vendor_email_content",
    # Put strings here, like "/home/html/django_templates" or "C:/www/django/templates".
    # Always use forward slashes, even on Windows.
    # Don't forget to use absolute paths, not relative paths.
)

INSTALLED_APPS = (
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.sites',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    #'south',
    'yoolotto.ad',
    'yoolotto.coin',
    'yoolotto.communication',
    'yoolotto.coupon',
    'yoolotto.games',
    'yoolotto.legacy',
    'yoolotto.lottery',
    'yoolotto.promo',
    'yoolotto.rest',
    'yoolotto.user',
    #'yoolotto.fantasy_lottery',
    'yoolotto.analytics',
    'yoolotto.second_chance',
    'yoolotto.openx_adunits',
    'cronjobs',
    
    # Celery
    
    'kombu.transport.django', 
    'djcelery',
    # Uncomment the next line to enable the admin:
    'django.contrib.admin',

    # Uncomment the next line to enable admin documentation:
    # 'django.contrib.admindocs',
    
    
    #django-dubug-tool bar 
    # If you're using Django 1.7.x or later
    #'debug_toolbar.apps.DebugToolbarConfig',
    # If you're using Django 1.6.x or earlier
    #'debug_toolbar',
)

# A sample logging configuration. The only tangible logging
# performed by this configuration is to send an email to
# the site admins on every HTTP 500 error when DEBUG=False.
# See http://docs.djangoproject.com/en/dev/topics/logging for
# more details on how to customize your logging configuration.
LOGGING = {
    'version': 1,
    'disable_existing_loggers': False,
    'filters': {
        'require_debug_false': {
            '()': 'django.utils.log.RequireDebugFalse'
        }
    },
    'handlers': {
        'mail_admins': {
            'level': 'ERROR',
            'filters': ['require_debug_false'],
            'class': 'django.utils.log.AdminEmailHandler'
        },
        'error_uncaught': {
           'level':'ERROR',
           'class':'logging.handlers.RotatingFileHandler',
           'filename': LOG_BASE_PATH + '/yoolotto_error_uncaught.log',
           'maxBytes': 1024*1024*5, # 5 MB
           'backupCount': 5,
        },
    },
    'loggers': {
        'django.request': {
            'handlers': ['mail_admins'],
            'level': 'ERROR',
            'propagate': True,
        },
        'yoolotto.uncaught': {
            'handlers': ['error_uncaught'],
            'level': 'ERROR',
            'propagate': True
        },
    }
}

"""
CELERY
"""

import djcelery
djcelery.setup_loader()

BROKER_URL = "amqp://"
#BROKER_URL = "amqp://guest:guest@localhost:5672//"

"""
NOTIFICATIONS
"""
NOTIFICATIONS_ENABLED = True
NOTIFICATIONS_BASE = os.path.join(SITE_CONTAINER, "cert")

NOTIFICATIONS_APNS_KEY_NAME = "apns.dev.key"
NOTIFICATIONS_APNS_CERT_NAME = "apns.dev.pem"
NOTIFICATIONS_SANDBOX = True

DEFAULT_FROM_EMAIL = "prizes@yoolotto.com"
EMAIL_BACKEND = 'django.core.mail.backends.smtp.EmailBackend'
EMAIL_USE_TLS = True
EMAIL_HOST = 'smtp.gmail.com'
EMAIL_PORT = 587
EMAIL_HOST_USER = 'yoolottocoupons@gmail.com'
EMAIL_HOST_PASSWORD = 'Yoolotto@123'

#NOTIFICATIONS_GCM_KEY = "AIzaSyBhQvfef5lZXI8nK45QX2vosVWv-vVJgtY"
#NOTIFICATIONS_GCM_KEY = "AIzaSyBOwcOkkJ6LXco8a3yWdj66kmNhzKLQddI"
NOTIFICATIONS_GCM_KEY = "AIzaSyBBKPjeLAHnQ4WLhp1k-cErhHDgMrbw46E"
try:
    from settings_local import *
except:
    print "Failed to Initialized Local Settings (Using Deployment Settings)"
    pass

NOTIFICATIONS_APNS_KEY_PATH = os.path.join(NOTIFICATIONS_BASE, NOTIFICATIONS_APNS_KEY_NAME)
NOTIFICATIONS_APNS_CERT_PATH = os.path.join(NOTIFICATIONS_BASE, NOTIFICATIONS_APNS_CERT_NAME)

'''import ox3apiclient

email = 'jimmy@yoolotto.com'
password = 'Shae1974'
domain = 'ox-ui.yoolotto.com'
realm = 'yoolotto'
consumer_key = '9d975f29a35fdd0f1002f373b529b0d6c492936c'
consumer_secret = '65ecca223b1d3b5470246951afc8eb12cbd45345'
ox = ox3apiclient.Client(
    email=email,
    password=password,
    domain=domain,
    realm=realm,
    consumer_key=consumer_key,
    consumer_secret=consumer_secret,
    api_path='/ox/4.0'
    )

#AFTER_LOGON_OX=ox.logon(email, password)
AFTER_LOGON_OX = None'''

SECOND_CHANCE_WINNING_MESSAGE = 'Prize sent to address, Please visit the help centre for ?'
SECOND_CHANCE_LOOSING_MESSAGE = 'Better Luck Next Time'

GAME_NUMBER = 2

INVITE_LIMIT = 10
INVITE_COINS = 2.5

MESSAGE_LIMIT = 10
MESSAGE_COINS = 2.5

ANDROID_GAME_MIN_NUMBER = 1
ANDROID_GAME_MAX_NUMBER = 3

SOCIAL_MESSAGE = 'Come and join Yoolotto!'

OPENX_SITE_ID = '537119731'

SCA_KEY = '057e6f5c-f440-44d5-bd39-453db780b54c'
