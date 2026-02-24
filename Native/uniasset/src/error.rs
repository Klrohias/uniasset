use std::{cell::RefCell, ffi::CString, fmt::Display};

thread_local! {
    static ERROR_INFO: RefCell<Option<ErrorInfo>> = RefCell::new(None);
}

#[derive(Clone)]
pub struct ErrorInfo {
    pub msg: CString,
}

pub fn has_error() -> bool {
    ERROR_INFO.with_borrow(|it| it.is_some())
}

pub fn with_error<T>(f: impl FnOnce(&Option<ErrorInfo>) -> T) -> T {
    ERROR_INFO.with_borrow(|it| f(it))
}

pub fn set_error(error: impl Display) {
    let message = CString::new(format!("{error}")).unwrap();
    ERROR_INFO.replace(Some(ErrorInfo { msg: message }));
}

pub fn clear_error() {
    ERROR_INFO.set(None);
}
