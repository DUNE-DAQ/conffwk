  /**
   *  \file Errors.h This file contains exception classes,
   *  which can be thrown by methods of conffwk packages.
   *  \author Igor Soloviev
   *  \brief exceptions of methods of conffwk packages
   */

#ifndef CONFFWK_ERRORS_H_
#define CONFFWK_ERRORS_H_

#include "ers/Issue.hpp"


namespace dunedaq {

    /**
     * \class dunedaq::conffwk::Exception
     * \brief Base class for all \b conffwk exceptions
     *
     *	This exception normally should be caught when any conffwk method is used.
\code   
try {
  Configuration db(...);
  ... // any user code working with db
}
catch (dunedaq::conffwk::Exception & ex) {
    // throw some user-defined exception in case of conffwk exception
  throw ers::error(user::exception(ERS_HERE, "conffwk database problem", ex));
}
\endcode
     */

  ERS_DECLARE_ISSUE_HPP( conffwk, Exception, , )


    /**
     * \class dunedaq::conffwk::Generic
     * \brief Generic configuration exception.
     *
     *	It reports most of the conffwk problems, such as:
     *  - bad database,
     *  - wrong parameter,
     *  - plug-in specific problems
     */

  ERS_DECLARE_ISSUE_BASE_HPP(
    conffwk,
    Generic,
    conffwk::Exception,
    ,
    ,
    ((const char*)what)
  )


    /**
     * \class dunedaq::conffwk::NotFound
     * \brief Try to access non-existent object or class.
     *
     *	It is thrown if a conffwk object accessed by ID is not found,
     *  or a class accessed by name is not found.
     */

  ERS_DECLARE_ISSUE_BASE_HPP(
    conffwk,
    NotFound,
    conffwk::Exception,
    ,
    ,
    ((const char*)type)
    ((const char*)data)
  )

    /**
     * \class dunedaq::conffwk::DeletedObject
     * \brief Try to access deleted DAL object.
     *
     *	It is thrown when a deleted DAL template object is accessed.
     *  Note, an object can be via notification mechanism or by the
     *  user's code modifying database.
     */

  ERS_DECLARE_ISSUE_BASE_HPP(
    conffwk,
    DeletedObject,
    conffwk::Exception,
    ,
    ,
    ((const char*)class_name)
    ((const char*)object_id)
  )

}

#endif
