
--CREATE TABLE IF NOT EXISTS model (
--    did INTEGER,
--    name TEXT,
--    PRIMARY KEY (did) ON CONFLICT REPLACE
--);
 
--CREATE TABLE IF NOT EXISTS type (
--    name TEXT,
--    size INTEGER
--    PRIMARY KEY (name) ON CONFLICT REPLACE
--);

-- a point defined in a data model    
--CREATE TABLE IF NOT EXISTS point (
--    name TEXT,
--    type INTEGER,
--    model INTEGER,      -- sunspec data model
--    units TEXT,
--    description TEXT
--    FOREIGN KEY (type) REFERENCES type(rowid),
--    FOREIGN KEY (model) REFERENCES model(did),
--    PRIMARY KEY (model, name) ON CONFLICT REPLACE
--);

CREATE TABLE IF NOT EXISTS device (
    unixtime INTEGER,
    usec INTEGER,
    cid INTEGER,
    id TEXT,
    iface TEXT,
    man TEXT,
    mod TEXT,
    ns TEXT,
    sn TEXT
    -- define a unique d record (this is expensive)
    -- PRIMARY KEY (unixtime,usec,id,man,mod,sn) ON CONFLICT REPLACE
);

-- corresponds to 'm' in the logger xml
CREATE TABLE IF NOT EXISTS dataset (
    model INTEGER,
    unixtime INTEGER,
    usec INTEGER,
    ns TEXT,
    x INTEGER,
    device INTEGER,
    FOREIGN KEY (model) REFERENCES model(did),
    FOREIGN KEY (device) REFERENCES device(rowid)
);

-- corresponds to 'p' in the logger xml
CREATE TABLE IF NOT EXISTS value (
    unixtime INTEGER,
    usec INTEGER,
    name TEXT,
    v TEXT,
    type INTEGER,
    sf INTEGER,
    x INTEGER,
    dataset INTEGER,
    FOREIGN KEY (dataset) REFERENCES rowid(dataset)
);

