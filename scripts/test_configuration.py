#!/usr/bin/env python3
# Andre dos Anjos <andre.dos.anjos@cern.ch>

"""Unit test for the Python bindings to the Configuration class."""

import os
import unittest
import conffwk

scriptsdir=os.path.dirname(os.path.realpath(__file__))

class Configuration(unittest.TestCase):
    """Tests if we can manipulate ConfigurationWrap objects as expected."""

    def test01_CanCreateDB(self):
        db = conffwk.Configuration("oksconflibs")
        db.create_db("test.data.xml", [f'{scriptsdir}/test.schema.xml'])
        db.commit()

    def test01a_DoesNotCrashIfDBNotThere(self):
        self.assertRaises(RuntimeError, conffwk.Configuration, "test2.data.xml")

    def test02_CanReopenDB(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        includes = db.get_includes("test.data.xml")
        self.assertEqual(len(includes), 1)
        self.assertEqual(includes[0], f"{scriptsdir}/test.schema.xml")

    def test03_CanAddIncludes(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        db.add_include(f'{scriptsdir}/core.schema.xml')
        includes = db.get_includes("test.data.xml")
        print(includes)
        self.assertEqual(len(includes), 2)
        db.commit()
        includes = db.get_includes("test.data.xml")
        self.assertEqual(len(includes), 2)

    def test04_CanRemoveIncludes(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        includes = db.get_includes()
        self.assertEqual(len(includes), 2)
        db.remove_include(includes[1])
        db.commit()
        includes = db.get_includes()
        self.assertEqual(len(includes), 1)

    def test05_CanCreateObject(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        for i in range(10):
            db.create_obj("Dummy", "TestDummy-%d" % i)
        db.commit()

    def test05a_CanCreateObjectFromOtherObject(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        master = db.create_obj("Dummy", "MasterDummy")
        for i in range(100, 110):
            db.create_obj("Dummy", "TestDummy-%d" % i)
        db.commit()

    def test06_CanTestForObjects(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        for i in range(10):
            self.assertTrue(db.test_object("Dummy", "TestDummy-%d" % i, 0, []))
        for i in range(1000, 1010):
            self.assertTrue(not db.test_object("Dummy", "TestDummy-%d" % i, 0, []))

    def test07_DetectsExistingObjects(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        self.assertRaises(RuntimeError, db.create_obj, "Dummy", "TestDummy-3")

    def test08_CanGetObject(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        obj = db.get_obj("Dummy", "TestDummy-4")

    def test08a_CanGetObjects(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        objs = db.get_objs("Dummy")
        self.assertEqual(len(objs), 21)

    def test09_DoesNotCrashIfNonExisting(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        self.assertRaises(RuntimeError, db.get_obj, "Dummy", "TestDummy-44")

    def test10_CanRemoveObject(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        obj = db.get_obj("Dummy", "TestDummy-1")
        db.destroy_obj(obj)
        db.commit()
        self.assertRaises(RuntimeError, db.get_obj, "Dummy", "TestDummy-1")

    def test11_CanCycleThroughStates(self):
        db = conffwk.Configuration("oksconflibs:test.data.xml")
        self.assertTrue(db.loaded())
        db.unload()
        self.assertTrue(not db.loaded())
        db.load("test.data.xml")
        self.assertTrue(db.loaded())
        db.unload()
        self.assertTrue(not db.loaded())

    def test12_CanStuffThousands(self):
        amount = 10000
        db = conffwk.Configuration("oksconflibs")
        db.create_db("test.data.xml", [f'{scriptsdir}/test.schema.xml'])
        for i in range(amount):
            db.create_obj("Dummy", "TestDummy-%d" % i)
        db.commit()

    def test13_CanCommitDeepRelations(self):
        import sys
        sys.setrecursionlimit(10000)

        depth = 10000
        db = conffwk.Configuration("oksconflibs")
        db.create_db("test.data.xml", [f'{scriptsdir}/test.schema.xml'])
        previous = None
        for i in range(depth):
            obj = db.create_obj("Second", "Object-%d" % i)
            if previous:
                obj['Another'] = previous
            previous = obj
        db.commit()


    # If CanRetrieveDeepRelations fails, don't expect subsequent tests to be able to open oksconflibs:test.data.xml

    def test14_CanRetrieveDeepRelations(self):
        # we test if one can leave the rlevel in gets() to "0" and that works
        import sys
        sys.setrecursionlimit(10000)

        depth = 10000
        db = conffwk.Configuration('oksconflibs:test.data.xml')
        # gets the topmost obj.
        obj = db.get_obj('Second', 'Object-%d' % (depth-1))
        counter = 1

        while obj['Another']:
            counter += 1
            obj = obj['Another']  # go deep in the relationship
        self.assertEqual(counter, depth)

    def test15_CommitWithComment(self):
        db = conffwk.Configuration("oksconflibs")
        dbfile = 'testcomment.data.xml'
        db.create_db(dbfile, [f'{scriptsdir}/test.schema.xml'])
        comment = "My test comment"
        db.commit(comment)
        del db
        with open(dbfile) as textfile:
            line = [l for l in textfile if ' <comment ' in l][0]

        element = line.splitlines()[0]
        self.assertTrue(comment in element)


if __name__ == "__main__":
    import sys
    sys.argv.append('-v')
    unittest.main()
