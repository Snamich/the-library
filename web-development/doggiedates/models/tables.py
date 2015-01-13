# coding: utf8

from datetime import datetime

def get_email():
    if auth.user:
        return auth.user.email
    else:
        return 'None'

#-----------------------------------------------------------------

# pet table
db.define_table('pet',
                Field('username', default = get_email()), # who does the pet belong to
                Field('pet_name', 'string', label = 'Name', required = True),
                Field('breed', 'string', label = 'Breed', required = True),
                Field('picture', 'upload', requires = IS_IMAGE(extensions=('jpeg', 'png'))),
                Field('personality', 'text', required = True),
                Field('hobbies', 'text', required = True),
)

# set placeholders for pet form
db.pet.pet_name.widget = lambda f,v: SQLFORM.widgets.string.widget(f, v, _placeholder='Jeff')
db.pet.breed.widget = lambda f,v: SQLFORM.widgets.string.widget(f, v, _placeholder='Corgi')
db.pet.personality.widget = lambda f,v: SQLFORM.widgets.string.widget(f, v, _placeholder='energetic')
db.pet.hobbies.widget = lambda f,v: SQLFORM.widgets.string.widget(f, v, _placeholder='chasing cats')

# properties setup
db.pet.id.readable = db.pet.writable = False
db.pet.username.readable = db.pet.username.writable = False

#-----------------------------------------------------------------

# greeting table
db.define_table('greeting',
                Field('username_from', required = True, default = get_email()),
                Field('pet_name_from', 'string', required = True, label = 'From'),
                Field('username_to', required = True, label = 'To User'),
                Field('pet_name_to', 'string', required = True, label = 'To'),
                Field('greeting', 'text', required = True),
)

# set placeholders for greeting form
db.greeting.pet_name_from.widget = lambda f,v: SQLFORM.widgets.string.widget(f, v, _placeholder='Enter your pet\'s name')
db.greeting.pet_name_to.widget = lambda f,v: SQLFORM.widgets.string.widget(f, v, _placeholder='Enter their pet\'s name')
db.greeting.greeting.widget = lambda f,v: SQLFORM.widgets.string.widget(f, v, _placeholder='Say hello!')

# properties setup
db.greeting.id.readable = db.greeting.id.writable = False
db.greeting.username_from.readable = db.greeting.username_from.writable = False
db.greeting.username_to.readable = db.greeting.username_to.writable = False
db.greeting.username_to.requires = [IS_EMAIL(error_message='Invalid email'),
                                    IS_IN_DB(db, db.auth_user.email, error_message='Not a valid user.')]

#-----------------------------------------------------------------

db.define_table('doggiedate',
                Field('username1', required = True, readable = False, writable = False),
                Field('pet1', 'string', required = True, readable = False, writable = False),
                Field('username2', required = True, readable = False, writable = False),
                Field('pet2', 'string', required = True, readable = False, writable = False),
                Field('date_time', 'datetime', required = True, requires = IS_DATETIME()),
                Field('latitude', readable = False, writable = False),
                Field('longitude', readable = False, writable = False),
                Field('accepted', 'boolean', default = False),
)

db.doggiedate.id.readable = db.doggiedate.id.writable = False
db.doggiedate.date_time.represent = lambda value, row: value.strftime("%d/%m/%Y %I:%M %p")
db.doggiedate.accepted.writable = db.doggiedate.accepted.readable = False
