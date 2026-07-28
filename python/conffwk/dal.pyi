"""
Contains the base class for DAL types and auxiliary methods.

This module defines the PyDALBase class that is used as the base type for all
DAL classes. A few utilities are also available.
"""
from __future__ import annotations
import typing
__all__: list[str] = ['DalBase', 'DalType', 'FSMAction', 'FSMData', 'FSMTransition', 'FSMxTransition', 'generate', 'get_classes', 'module', 'prettyprint_cardinality', 'prettyprint_doc', 'prettyprint_range']
class DalBase:
    """
    This class is used to represent any DAL object in the system. 
    """
    _updated: typing.ClassVar[set] = set()
    @staticmethod
    def reset_updated_list():
        """
        Reset the set keeping track of modified DAL objects
                
        """
    @staticmethod
    def updated():
        """
        Returns a set of DAL objects that were modified in this DB session
                
        """
    def __eq__(self, other):
        """
        True is the 2 objects have the same class and ID.
        """
    def __ge__(self, other):
        """
        True if the object is greater or equal than the other
                alphabetically.
                
        """
    def __getall__(self, comp = None):
        """
        Get all relations, includding a link to myself
        """
    def __getattr__(self, par):
        """
        Returns a given attribute or relationship.
        
                This method returns an attribute or relationship from the current
                object, or throws an AttributeError if no such thing exists. It sets
                the field touched, so it does not get called twice.
                
        """
    def __gt__(self, other):
        """
        True if the object is greater than the other alphabetically.
        """
    def __hash__(self):
        """
        This method is meant to be used to allow DAL objects as map keys.
        """
    def __init__(self, id, **kwargs):
        """
        Constructs an object by setting its id (UID in OKS jargon) at least.
        
                This method will initialize an object of the DalBase type, by setting
                its internal properties (with schema cross-checking where it is
                possible). The user should at least set the object's id, which at this
                moment is not checked for uniqueness.
        
                Keyword arguments:
        
                id -- This is the unique identifier (per database) that the user wants
                to assign to this object. This identifier will be used as the OKS
                identifier when and if this object is ever serialized in an OKS
                database.
        
                **kwargs -- This is a set of attributes and relationships that must
                exist in the associated DAL class that inherits from this base.
                
        """
    def __le__(self, other):
        """
        Returns True if the class is smaller or equal than the other. 
        """
    def __lt__(self, other):
        """
        True if the class is smaller than the other.  
        """
    def __ne__(self, other):
        """
        True if the 2 objects *not* have the same class and ID.
        """
    def __repr__(self):
        """
        Returns a nice representation of this object.
        """
    def __reset_identity__(self):
        ...
    def __setattr__(self, par, val):
        """
        Sets an object attribute or relationship.
        
                This method overrides the default setattr method, so it can apply
                existence and type verification on class attributes. If the attribute
                to be set starts with '__', or the passed value is None,
                no verification is performed. If the value to set an attribute is a
                list, the type verification is performed in every component of that
                list.
        
                N.B.: This method takes a reference to the object being passed. It does
                not copy the value, so, if you do a.b = c, and then you apply changes
                to 'c', these changes will be also applied to 'a.b'.
        
                Parameters:
        
                par -- The name of the parameter (attribute or relationship)
        
                val -- The value that will be attributed to 'par'.
        
                Raises AttributeError if the parameter does not exist.
        
                Raises ValueError if the value you passed cannot be coerced to a
                compatible OKS python type for the attribute or relationship you are
                trying to set.
                
        """
    def __str__(self):
        """
        Returns human readable information about the object.
        """
    def className(self):
        ...
    def copy(self, other):
        """
        Copies attributes and relationships from the other component.
        
                This will copy whatever relevant attributes and relationships from
                another component into myself. The implemented algorithm starts by
                iterating on my own schema and looking for the counter part on the
                other class's schema, only matching values are copied. This is useful
                to copy values from base class objects or templated class objects.
        
                Arguments:
        
                other -- This is the other dal object you are trying to copy.
                
        """
    def fullName(self):
        ...
    def get(self, className, idVal = None, lookBaseClasses = False):
        """
        Get components in the object based on class name and/or id.
        
                This method runs trough the components of its relationships and
                returns a sorted list (sorting based on class name and object ID)
                containing references to all components that match the search criteria.
        
                Keyword Parameters (may be named):
        
                className -- The name of the class to look for. Should be a string
        
                idVal -- The id of the object to look for. If not set (or set to None),
                the search will be based only on the class name. If set, it must be
                either a string or an object that defines a match() method (such as a
                regular expression).
        
                lookBaseClasses -- If True and parameter to be search is a class, the
                method will look also through the base classes names, so if value =
                Application, for instance the method will return all objects of class
                Application or that inherit from the Application class.
        
                Returns a list with all the components that matched the search
                criteria, if idVal is not set or is a type that defines a match()
                method such as a regular expression. Otherwise (if it is a string)
                returns a single object, if any is found following the criterias for
                className and a exact idVal match.
                
        """
    def isDalType(self, val):
        ...
    def oksTypes(self):
        ...
    def rename(self, new_name):
        """
        
                Rename the DAL object to a new name.
        
                This will store the old name in a hidden attribute and when
                Configuration.update_dal() is called we use it to check
                if there is an existing object with that name in the database.
        
                If yes, we call the underlying ConfigObject.rename() method
                transparently. If the old name does not exist in the database,
                nothing special is done.
        
                This is the only 'official' way to rename an object on the
                DAL level. Just changing the 'id' attribute will not have
                the same effect.
                
        """
    def setattr_nocheck(self, par, val):
        """
        Sets an attribute by-passing the built-in type check.
        """
class DalType(type):
    """
    
        This class is the metaclass that every DAL class will be created with.
    
        DalType is a metaclass (something like a C++ template) that allows us to
        create DAL classes without having to go through the 'exec' burden all the
        time and being, therefore, much faster than that mechanism. The idea is
        that we create DAL types everytime we see a new class and archive this in a
        cache, together with the configuration object. Everytime an object of a
        certain OKS type is needed by the user, we make use of the generated class
        living in that cache to make it a new DAL object.
    
        The DAL type consistency checks are limited by the amount of generic
        functionality one can extract by looking at the C++ Configuration class.
    
        The work here is modelled after the old PyDALBase implementation that used
        to live in the "genconffwk" package.
        
    """
    @classmethod
    def __init__(cls, name, bases, dct):
        """
        Class constructor.
        
                Keyword Parameters:
        
                cls -- This is a pointer to the class being constructed
        
                name -- The name that the class will have
        
                bases -- These are the classes, objects of the new generated type will
                inherit from. It is useful in our context, to express the OKS
                inheritance relations between the classes.
        
                dct -- This is a dictionary that will contain mappings between
                methods/attributes of the newly generated class and values or methods
                that will be bound to it. The dictionary should contain a pointer to
                the class schema, and that should be called '__schema__'.
                
        """
    @classmethod
    def pyclassName(cls):
        """
        Returns this class name
        """
    @classmethod
    def pyoksTypes(cls):
        """
        Returns a join of this class and base class names
        """
class FSMAction(DalBase):
    """
    This class is used to represent any FSM action object in the system. 
    """
class FSMData(DalBase):
    """
    This class is used to represent any FSM data object in the system. 
    """
class FSMTransition(DalBase):
    """
    This class is used to represent any FSM transition object in the system. 
    """
class FSMxTransition(DalBase):
    """
    This class is used to represent any FSM pre/post transition object in the system. 
    """
def __recmp__(pat, v):
    ...
def __strcmp__(v1, v2):
    ...
def generate(configuration, other_dals = list()):
    """
    Generates the DAL python access layer for the configuration passed.
    
        This method will generate the python DAL access layer for all classes
        declared through the conffwk.Configuration object passed. If this file
        includes other schemas, the classes for those schemas will also be
        generated, unless, classes with matching names are passed through the
        "other_dals" parameters.
    
        This method will re-use classes generated in other calls to this method,
        either directly (in DAL binding to a python module) or while you created
        Configuration type objects. So, you can call this as many times as you want
        without incurring in much overhead.
    
        Keyword parameters:
    
        configuration -- The conffwk.Configuration object that you want the prepare
        the DAL for.
    
        other_dals -- This is a list of classes that contain other DALs that should
        be considered for the inheritance structure of the classes that are going
        to be generated here. These classes will not be regenerated. This parameter
        can be either a list of modules or classes that won't be regenerated, but
        re-used by this generation method.
    
        Returns the DAL classes you asked for.
        
    """
def get_classes(m):
    """
    Returns a map with classes in a module, the key is the class name.
    """
def module(name, schema, other_dals = list(), backend = 'oksconflibs', db = None):
    """
    Creates a new python module with the OKS schema files passed as
        parameter.
    
        This method creates a new module for the user, using the schema files
        passed as parameter. Classes from other DALs are not re-created, but just
        re-used. This is an example usage:
    
        import conffwk.dal
        dal = conffwk.dal.module('dal', 'dal/schema/core.schema.xml')
        DFdal = conffwk.dal.module('DFdal', 'DFConfiguration/schema/df.schema.xml',
                                  [dal])
    
        This will generate two python dals in the current context. One that binds
        everything available in the first schema file and a second one that binds
        everything else defined in the DF OKS schema file.
    
        Keyword parameters:
    
        name -- The name of the python module to create. It should match the name
        of the variable you are attributing to, but it is not strictly required by
        the python interpreter, just a good practice.
    
        schema -- This is a list of OKS schema files that should be considered. You
        can also pass OKS datafiles to this one, which actually includes the schema
        files you want to have a DAL for. It will just work.
    
        other_dals -- This is a list of other DAL modules that I'll not regenerate,
        and which classes will *not* make part of the returned module. In fact,
        this parameter is only used to restrict the amount of output classes since
        once class is generated internally, it is not regenerated a second time.
        In other words creating twice the same DAL implies in almost no overhead.
    
        backend -- This is the OKS backend to use when retrieving the schemas. By
        default it is set to 'oksconflibs', which is what we
        
    """
def prettyprint_cardinality(not_null, multivalue):
    """
    Returns a nice string representation for an object cardinality
    """
def prettyprint_doc(entry):
    """
    Pretty prints a schema Cache entry, to be used by __doc__ strings
    """
def prettyprint_range(attr):
    """
    Prints the range of an attribute in a nice way
    """
__dal__: dict = {}
