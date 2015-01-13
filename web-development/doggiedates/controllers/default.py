# -*- coding: utf-8 -*-
# this file is released under public domain and you can use without limitations

#########################################################################
## This is a sample controller
## - index is the default action of any application
## - user is required for authentication and authorization
## - download is for downloading files uploaded in the db (does streaming)
## - call exposes all registered services (none by default)
#########################################################################

from sets import Set

#---------------------------------------------------------------

@auth.requires_login()
def index():
    if auth.user:
        redirect(URL('default', 'home'))
        
    return dict()

#---------------------------------------------------------------

@auth.requires_login()
def add_pet():
    form = SQLFORM(db.pet,buttons = [TAG.button('Add Pet',_type='submit'),
                                     TAG.button('Cancel',_type="button",_onClick = "parent.location='%s' " % URL('default', 'list_greeting'))])
    if form.process().accepted:
        redirect(URL('default', 'view_pet', args=[form.vars.id, 'list_pet']))
    return dict(form=form)

@auth.requires_login()
def list_pet():
    q = db.pet.username == auth.user.email

    # only let them edit picture, hobbies, and availability
    db.pet.pet_name.writable = db.pet.breed.writable = False

    form = SQLFORM.grid(q,
                        searchable=False, # should only have a couple pets, no need for searching
                        fields=[db.pet.pet_name], # only the name is really relevant at this point
                        csv=False,
                        links=[lambda row: A('View',_href=URL('default', 'view_pet' ,args=[row.id, 'list_pet']))],
                        details=False, create=False, editable=True, deletable=True,)

    return dict(form=form)

@auth.requires_login()
def view_pet():
    record = db.pet[request.args(0)]
    rc = request.args(1)
    
    return dict(record=record, rc=rc)

#---------------------------------------------------------------

@auth.requires_login()
def add_greeting():
    record = db.pet[request.args(0)]
    db.greeting.pet_name_to.default = record.pet_name
    db.greeting.username_to.default = record.username

    # look up the sender's pets for autofilling
    names = []
    for row in db(db.pet.username == auth.user.email).select():
        names.append(row.pet_name)
        
    # enable dropdown menu selection for the sender's pet
    db.greeting.pet_name_from.widget = SQLFORM.widgets.options.widget
    db.greeting.pet_name_from.requires = IS_IN_SET(names, zero=T('Choose your pet'))

    form = SQLFORM(db.greeting, buttons = [TAG.button('Send!',_type='submit'),
                                           TAG.button('Cancel',_type="button",_onClick = "parent.location='%s' " % URL('default', 'list_dog'))])

    if form.process().accepted:
        redirect(URL('default', 'list_dog'))
    return dict(form=form)

@auth.requires_login()
def list_greeting():
    q = db.greeting.username_to == auth.user.email

    form = SQLFORM.grid(q,
                        searchable=True,
                        fields=[db.greeting.username_from, db.greeting.pet_name_from],
                        csv=False,
                        links=[lambda row: A('View',_href=URL('default', 'view_greeting' ,args=[row.id, 'list_greeting']))],
                        details=False, create=False, editable=False, deletable=True,)

    return dict(form=form)

@auth.requires_login()
def view_greeting():
    record = db.greeting[request.args(0)]
    rc = request.args(1)

    # look up the information for the sender's pet
    pet = db(db.pet.username == record.username_from and
             db.pet.pet_name == record.pet_name_from).select()[0]

    return dict(record=record, pet=pet, rc=rc)

#---------------------------------------------------------------

@auth.requires_login()
def add_date():
    record = db.greeting[request.args(0)]

    pet1 = db(db.pet.username == record.username_from and
              db.pet.pet_name == record.pet_name_from).select()[0]
    
    pet2 = db(db.pet.username == record.username_to and
              db.pet.pet_name == record.pet_name_to).select()[0]

    # prepopulate the date fields we have
    db.doggiedate.pet1.default = record.pet_name_to
    db.doggiedate.username1.default = record.username_to
    db.doggiedate.pet2.default = record.pet_name_from
    db.doggiedate.username2.default = record.username_from

    form = SQLFORM(db.doggiedate, buttons = [TAG.button('Add location',_type='submit'),
                                             TAG.button('Cancel',_type="button",_onClick = "parent.location='%s' " % URL('default', 'home'))])

    if form.process().accepted:
        redirect(URL('default', 'add_location', args=form.vars.id))

    return dict(form=form, pet1=pet1, pet2=pet2)

@auth.requires_login()
def add_location():
    return dict(id=request.args(0))

@auth.requires_login()
def set_location():
    item = db.doggiedate[request.post_vars.id]
    lat = request.vars.lat
    lng = request.vars.lng

    item.update_record(latitude=lat)
    item.update_record(longitude=lng)

    return str(request.post_vars.id)

@auth.requires_login()
def list_date():
    q = (db.doggiedate.username2 == auth.user.email) or (db.doggiedate.username2 == auth.user.email)

    db.doggiedate.pet1.readable = db.doggiedate.pet2.readable = db.doggiedate.accepted.readable = True

    form = SQLFORM.grid(q,
                        searchable=True,
                        fields=[db.doggiedate.pet1, db.doggiedate.pet2, 
                                db.doggiedate.accepted, db.doggiedate.date_time],
                        csv=False,
                        links=[lambda row: A('View',_href=URL('default', 'view_date' ,args=[row.id, 'list_date']))],
                        details=False, create=False, editable=False, deletable=True,)

    return dict(form=form)

@auth.requires_login()
def view_date():
    record = db.doggiedate[request.args(0)]

    pet1 = db(db.pet.username == record.username1 and
              db.pet.pet_name == record.pet1).select()[0]

    pet2 = db(db.pet.username == record.username2 and
              db.pet.pet_name == record.pet2).select()[0]

    db.doggiedate.date_time.writable = db.doggiedate.date_time.readable = False
    db.doggiedate.latitude.writable = db.doggiedate.latitude.readable = False
    db.doggiedate.longitude.writable = db.doggiedate.longitude.readable = False    
    db.doggiedate.accepted.writable = True

    form = SQLFORM(db.doggiedate, record)

    if form.process().accepted:
        session.flash = 'date accepted!'
        redirect(URL('default', 'list_date'))
    
    return dict(record=record, pet1=pet1, pet2=pet2, form=form, user=auth.user.email)

#---------------------------------------------------------------

@auth.requires_login()
def list_dog():
    # check if the user has any pets
    records = db(db.pet.username == auth.user.email).select()
    if not records:
        # if not, tell them they must add a pet before looking for some
        session.flash = 'you must have a pet before you can look for a date'
        redirect(URL('default', 'home'))

    # don't want to list any of the user's pets
    q = db.pet.username != auth.user.email

    form = SQLFORM.grid(q,
                        searchable=True,
                        fields=[db.pet.pet_name, db.pet.breed],
                        csv=False,
                        links=[lambda row: A('View',_href=URL('default', 'view_pet', args=[row.id, 'list_dog']))],
                        details=False, create=False, editable=False, deletable=False,)

    return dict(form=form)

#---------------------------------------------------------------

@auth.requires_login()
def home():
    return dict()

#---------------------------------------------------------------

def user():
    """
    exposes:
    http://..../[app]/default/user/login
    http://..../[app]/default/user/logout
    http://..../[app]/default/user/register
    http://..../[app]/default/user/profile
    http://..../[app]/default/user/retrieve_password
    http://..../[app]/default/user/change_password
    http://..../[app]/default/user/manage_users (requires membership in
    use @auth.requires_login()
        @auth.requires_membership('group name')
        @auth.requires_permission('read','table name',record_id)
    to decorate functions that need access control
    """
    return dict(form=auth())

@cache.action()
def download():
    """
    allows downloading of uploaded files
    http://..../[app]/default/download/[filename]
    """
    return response.download(request, db)


def call():
    """
    exposes services. for example:
    http://..../[app]/default/call/jsonrpc
    decorate with @services.jsonrpc the functions to expose
    supports xml, json, xmlrpc, jsonrpc, amfrpc, rss, csv
    """
    return service()


@auth.requires_signature()
def data():
    """
    http://..../[app]/default/data/tables
    http://..../[app]/default/data/create/[table]
    http://..../[app]/default/data/read/[table]/[id]
    http://..../[app]/default/data/update/[table]/[id]
    http://..../[app]/default/data/delete/[table]/[id]
    http://..../[app]/default/data/select/[table]
    http://..../[app]/default/data/search/[table]
    but URLs must be signed, i.e. linked with
      A('table',_href=URL('data/tables',user_signature=True))
    or with the signed load operator
      LOAD('default','data.load',args='tables',ajax=True,user_signature=True)
    """
    return dict(form=crud())
