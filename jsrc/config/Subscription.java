package config;

import java.util.Map.Entry;
import java.util.TreeSet;
import java.util.Hashtable;

/**
 * The class is used to provide the subscription criteria.
 * It the subscription criteria is empty, it matches any database changes, i.e. the callback
 * associated with such subscription will be invoked for any modification of database.
 * Otherwise the subscription criteria can be changed via two methods described below:
 * <ul>
 * <li><code>{@link #add(String class_name)}</code> - add subscription on any changes in given class
 * <li><code>{@link #add(String class_name, String object_id)}</code> - add subscription on modification or removing given object
 * </ul>
 * The {@link #print(String) print()} prints out details of given subscription and can be used for debug purposes.
 * 
 * @author http://consult.cern.ch/xwho/people/432778
 * @since online release 00-21-00
 */

public class Subscription
  {
    /** User callback object (is used by the Configuration class). */
    protected Callback m_cb;

    /** User callback parameter (is used by the Configuration class). */
    protected Object m_parameter;

    /** Set of class subscriptions (is used by the Configuration class). */
    protected TreeSet<String> m_class_subscriptions;

    /** Map of objects subscriptions (is used by the Configuration class). */
    protected Hashtable<String, TreeSet<String>> m_obj_subscriptions;

    /**
     * Constructor is used by the configuration object.
     * 
     * @param cb
     *          a user callback object implementing {@link Callback} interface
     * @param param
     *          user-defined callback parameter
     */

    public Subscription(Callback cb, Object param)
      {
        m_cb = cb;
        m_parameter = param;
        m_class_subscriptions = null;
        m_obj_subscriptions = null;
      }

    /**
     * Subscribe on any changes in given class. The possible changes are
     * creation, modification or removing an object of this class or it's
     * subclasses.
     * 
     * @param class_name
     *          name of the class
     */

    public void add(String class_name)
      {
        if (m_class_subscriptions == null)
          {
            m_class_subscriptions = new TreeSet<String>();
          }

        m_class_subscriptions.add(class_name);
      }

    /**
     * Subscribe on object changes by class name and object identity. The
     * notification will be invoked in case if the object is modified or
     * removed.
     * 
     * @param class_name
     *          name of the object's class
     * @param object_id
     *          object's identity
     */

    public void add(String class_name, String object_id)
      {
        TreeSet<String> s = null;

        if (m_obj_subscriptions == null)
          {
            m_obj_subscriptions = new Hashtable<String, TreeSet<String>>();
          }
        else
          {
            s = m_obj_subscriptions.get(class_name);
          }

        if (s == null)
          {
            s = new java.util.TreeSet<String>();
            m_obj_subscriptions.put(class_name, s);
          }

        s.add(object_id);
      }

    /**
     * Subscribe on object changes for an object of class generated by
     * genconfig. The notification will be invoked in case if the object is
     * modified or removed.
     * 
     * @param obj
     *          an object of class generated by genconfig
     */

    public void add(DalObject obj)
      {
        add(obj.class_name(), obj.UID());
      }

    /**
     * Method to print out subscription criteria. To be used for debug purposes.
     * 
     * @param dx
     *          string for left margin (for a formatted output)
     */

    public void print(String dx)
      {
        System.out.println(dx + "subscription:");

        // print name and parameter

        System.out.println(dx + "  cb: " + m_cb);
        System.out.println(dx + "  parameter: " + m_parameter);

        // print class subscriptions

        System.out.print(dx + "  classes:");

        if ((m_class_subscriptions != null) && (m_class_subscriptions.size() > 0))
          {
            System.out.println();
            for (String i : m_class_subscriptions)
              {
                System.out.println(dx + "    \'" + i + "\'");
              }
          }
        else
          {
            System.out.println(" (null)");
          }

        // print object subscriptions

        System.out.print(dx + "  objects:");

        if ((m_obj_subscriptions != null) && (m_obj_subscriptions.size() > 0))
          {
            System.out.println();

            for (Entry<String, TreeSet<String>> entry : m_obj_subscriptions.entrySet())
              {
                System.out.println(dx + "    class \'" + entry.getKey() + "\':");
                for (String j : entry.getValue())
                  {
                    System.out.println(dx + "      \'" + j + "\'");
                  }
              }
          }
        else
          {
            System.out.println(" (null)");
          }
      }
  }
