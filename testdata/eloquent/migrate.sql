-- user.json

CREATE TABLE users (
    id    INTEGER PRIMARY KEY AUTOINCREMENT,
    name  VARCHAR(30) NOT NULL
);

CREATE TABLE phones (
    id      INTEGER PRIMARY KEY AUTOINCREMENT,
    number  VARCHAR(30) NOT NULL,
    user_id INTEGER NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE TABLE roles (
    id    INTEGER PRIMARY KEY AUTOINCREMENT,
    name  VARCHAR(30) NOT NULL
);

CREATE TABLE role_user(
    id      INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    role_id INTEGER NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(id)
    FOREIGN KEY (role_id) REFERENCES roles(id)
);

-- post.json
-- comment.json

CREATE TABLE posts (
    id   INTEGER PRIMARY KEY AUTOINCREMENT,
    text VARCHAR(255) NOT NULL
);

CREATE TABLE comments (
    id      INTEGER PRIMARY KEY AUTOINCREMENT,
    text    VARCHAR(255) NOT NULL,
    post_id INTEGER NOT NULL,
    FOREIGN KEY(post_id) REFERENCES posts(id)
);

-- application.json

CREATE TABLE applications(
    id    INTEGER PRIMARY KEY AUTOINCREMENT,
    name  VARCHAR(30) NOT NULL
);

CREATE TABLE environments (
    id             INTEGER PRIMARY KEY AUTOINCREMENT,
    name           VARCHAR(30) NOT NULL,
    application_id INTEGER NOT NULL,
    FOREIGN KEY(application_id) REFERENCES applications(id)
);

CREATE TABLE deployments (
    id             INTEGER PRIMARY KEY AUTOINCREMENT,
    commit_hash    VARCHAR(30) NOT NULL,
    environment_id INTEGER NOT NULL,
    FOREIGN KEY(environment_id) REFERENCES environments(id)
);

-- mechanic.json

