use std::{cell::UnsafeCell, ops::Deref};

/// Use `std::cell::SyncUnsafeCell` instead of this when `std::cell::SyncUnsafeCell` is stable.
pub struct SyncUnsafeCell<T: ?Sized>(UnsafeCell<T>);

unsafe impl<T: ?Sized> Sync for SyncUnsafeCell<T> {}
unsafe impl<T: ?Sized> Send for SyncUnsafeCell<T> {}

impl<T: ?Sized> Deref for SyncUnsafeCell<T> {
    type Target = UnsafeCell<T>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl<T> SyncUnsafeCell<T> {
    pub fn new(value: T) -> Self {
        Self(UnsafeCell::new(value))
    }
}

impl<T> From<T> for SyncUnsafeCell<T> {
    fn from(value: T) -> Self {
        Self::new(value)
    }
}
