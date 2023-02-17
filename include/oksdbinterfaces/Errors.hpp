  /**
   *  \file Errors.h This file contains exception classes,
   *  which can be thrown by methods of oksdbinterfaces packages.
   *  \author Igor Soloviev
   *  \brief exceptions of methods of oksdbinterfaces packages
   */

#ifndef OKSDB_INTERFACE_ERRORS_H_
#define OKSDB_INTERFACE_ERRORS_H_

#include "ers/Issue.hpp"


namespace dunedaq {

    /**
     * \class dunedaq::oksdbinterfaces::Exception
     * \brief Base class for all \b oksdbinterfaces exceptions
     *
     *	This exception normally should be caught when any oksdbinterfaces method is used.
\code   
try {
  Configuration db(...);
  ... // any user code working with db
}
catch (dunedaq::oksdbinterfaces::Exception & ex) {
    // throw some user-defined exception in case of oksdbinterfaces exception
  throw ers::error(user::exception(ERS_HERE, "oksdbinterfaces database problem", ex));
}
\endcode
     */

  ERS_DECLARE_ISSUE_HPP( oksdbinterfaces, Exception, , )


    /**
     * \class dunedaq::oksdbinterfaces::Generic
     * \brief Generic configuration exception.
     *
     *	It reports most of the oksdbinterfaces problems, such as:
     *  - bad database,
     *  - wrong parameter,
     *  - plug-in specific problems
     */

  ERS_DECLARE_ISSUE_BASE_HPP(
    oksdbinterfaces,
    Generic,
    oksdbinterfaces::Exception,
    ,
    ,
    ((const char*)what)
  )


    /**
     * \class dunedaq::oksdbinterfaces::NotFound
     * \brief Try to access non-existent object or class.
     *
     *	It is thrown if a oksdbinterfaces object accessed by ID is not found,
     *  or a class accessed by name is not found.
     */

  ERS_DECLARE_ISSUE_BASE_HPP(
    oksdbinterfaces,
    NotFound,
    oksdbinterfaces::Exception,
    ,
    ,
    ((const char*)type)
    ((const char*)data)
  )

    /**
     * \class dunedaq::oksdbinterfaces::DeletedObject
     * \brief Try to access deleted DAL object.
     *
     *	It is thrown when a deleted DAL template object is accessed.
     *  Note, an object can be via notification mechanism or by the
     *  user's code modifying database.
     */

  ERS_DECLARE_ISSUE_BASE_HPP(
    oksdbinterfaces,
    DeletedObject,
    oksdbinterfaces::Exception,
    ,
    ,
    ((const char*)class_name)
    ((const char*)object_id)
  )

}

#endif
