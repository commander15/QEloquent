-- System internals

CREATE TABLE DatabaseVersions (
    id    INTEGER PRIMARY KEY AUTOINCREMENT,
    major INTEGER NOT NULL,
    minor INTEGER NOT NULL,
    patch INTEGER DEFAULT 0,
    installed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO DatabaseVersions (major, minor, patch) VALUES
(1, 0, 0);

-- User management

CREATE TABLE UserRoles (
    id   INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR(30) UNIQUE NOT NULL
);

CREATE TABLE UserGroups (
    id     INTEGER PRIMARY KEY AUTOINCREMENT,
    name   VARCHAR(30) UNIQUE NOT NULL
);

CREATE TABLE Users (
    id       INTEGER PRIMARY KEY AUTOINCREMENT,
    name     VARCHAR(30) NOT NULL,
    email    VARCHAR(30) UNIQUE NOT NULL,
    password VARCHAR(60) NOT NULL,
    role_id  INTEGER NOT NULL,
    FOREIGN KEY (role_id) REFERENCES UserRoles(id)
);

CREATE TABLE UserGroupMembers (
    id       INTEGER PRIMARY KEY AUTOINCREMENT,
    group_id INTEGER NOT NULL,
    user_id  INTEGER NOT NULL,
    FOREIGN KEY (group_id) REFERENCES Groups(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id)  REFERENCES Users(id)  ON DELETE CASCADE,
    UNIQUE (group_id, user_id)
);

-- Product management

CREATE TABLE Categories (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        VARCHAR(30) NOT NULL,
    description VARCHAR(255),
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE Products (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        VARCHAR(30) NOT NULL,
    description VARCHAR(255),
    price       REAL NOT NULL CHECK (price >= 0),
    barcode     VARCHAR(64) UNIQUE,
    category_id INTEGER,
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(category_id) REFERENCES Categories(id) ON DELETE SET NULL
);

CREATE TABLE Stocks (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    quantity    INTEGER NOT NULL DEFAULT 0 CHECK (quantity >= 0),
    product_id  INTEGER NOT NULL,
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (product_id) REFERENCES Products(id) ON DELETE CASCADE
);

-- Sales management

CREATE TABLE Sales (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    number     INTEGER NOT NULL CHECK (number > 0),
    amount     REAL NOT NULL CHECK (amount >= 0),
    seller_id  INTEGER,   
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (seller_id) REFERENCES Users(id) ON DELETE SET NULL
);

CREATE TABLE SaleItems (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    unit_price REAL NOT NULL,
    quantity   INTEGER NOT NULL CHECK (quantity > 0),
    sale_id    INTEGER NOT NULL,
    product_id INTEGER,
    FOREIGN KEY (sale_id)    REFERENCES Sales(id)    ON DELETE CASCADE,
    FOREIGN KEY (product_id) REFERENCES Products(id) ON DELETE SET NULL
);

